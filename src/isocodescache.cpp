/*
    SPDX-FileCopyrightText: 2021 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "isocodes_p.h"
#include "isocodescache_p.h"
#include "ki18n_logging.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

// increment those when changing the format
enum : uint32_t {
    Iso3166_1CacheHeader = 0x4B493101,
    Iso3166_2CacheHeader = 0x4B493201,
};

static QString isoCodesPath(QStringView file)
{
#ifndef Q_OS_ANDROID
    return QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("iso-codes/json/") + file, QStandardPaths::LocateFile);
#else
    return QLatin1String("assets:/share/iso-codes/json/") + file;
#endif
}

static QString cachePath()
{
    return QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + QLatin1String("/org.kde.ki18n/iso-codes/");
}

static QString cacheFilePath(QStringView file)
{
    return cachePath() + file;
}

IsoCodesCache::~IsoCodesCache() = default;

IsoCodesCache *IsoCodesCache::instance()
{
    static IsoCodesCache s_cache;
    return &s_cache;
}

void IsoCodesCache::loadIso3166_1()
{
    if (!m_iso3166_1CacheData && !loadIso3166_1Cache()) {
        createIso3166_1Cache();
        loadIso3166_1Cache();
    }
}

bool IsoCodesCache::loadIso3166_1Cache()
{
    QFileInfo jsonFi(isoCodesPath(u"iso_3166-1.json"));
    auto f = std::make_unique<QFile>(cacheFilePath(u"iso_3166-1"));
    if (!f->open(QFile::ReadOnly) || f->fileTime(QFile::FileModificationTime) < jsonFi.lastModified() || f->size() < 8) {
        return false;
    }

    // validate cache file is usable
    // header matches
    const auto data = f->map(0, f->size());
    if (*reinterpret_cast<const uint32_t *>(data) != Iso3166_1CacheHeader) {
        return false;
    }
    // lookup tables fit into the available size
    const auto size = *(reinterpret_cast<const uint32_t *>(data) + 1);
    if (sizeof(Iso3166_1CacheHeader) + sizeof(size) + size * sizeof(MapEntry<uint16_t>) * 2 >= (std::size_t)f->size()) {
        return false;
    }
    // string table is 0 terminated
    if (data[f->size() - 1] != '\0') {
        return false;
    }

    m_iso3166_1CacheFile = std::move(f);
    m_iso3166_1CacheData = data;
    return true;
}

uint32_t IsoCodesCache::countryCount() const
{
    return m_iso3166_1CacheData ? *(reinterpret_cast<const uint32_t *>(m_iso3166_1CacheData) + 1) : 0;
}

const MapEntry<uint16_t> *IsoCodesCache::countryNameMapBegin() const
{
    return m_iso3166_1CacheData ? reinterpret_cast<const MapEntry<uint16_t> *>(m_iso3166_1CacheData + sizeof(uint32_t) * 2) : nullptr;
}

const MapEntry<uint16_t> *IsoCodesCache::countryAlpha3MapBegin() const
{
    return m_iso3166_1CacheData ? countryNameMapBegin() + countryCount() : nullptr;
}

const char *IsoCodesCache::countryStringTableLookup(uint16_t offset) const
{
    if (m_iso3166_1CacheData) {
        offset += 2 * sizeof(uint32_t) + 2 * countryCount() * sizeof(MapEntry<uint16_t>);
        return m_iso3166_1CacheFile->size() > offset ? reinterpret_cast<const char *>(m_iso3166_1CacheData + offset) : nullptr;
    }
    return nullptr;
}

void IsoCodesCache::createIso3166_1Cache()
{
    qCDebug(KI18N) << "Rebuilding ISO 3166-1 cache";
    const auto path = isoCodesPath(u"iso_3166-1.json");

    QFile file(path);
    if (!file.open(QFile::ReadOnly)) {
        qCWarning(KI18N) << "Unable to open iso_3166-1.json" << path << file.errorString();
        return;
    }

    std::vector<MapEntry<uint16_t>> alpha2NameMap;
    std::vector<MapEntry<uint16_t>> alpha3alpha2Map;
    QByteArray iso3166_1stringTable;

    const auto doc = QJsonDocument::fromJson(file.readAll());
    const auto array = doc.object().value(QLatin1String("3166-1")).toArray();
    for (const auto &entryVal : array) {
        const auto entry = entryVal.toObject();
        const auto alpha2 = entry.value(QLatin1String("alpha_2")).toString();
        if (alpha2.size() != 2) {
            continue;
        }
        const auto alpha2Key = IsoCodes::alpha2CodeToKey(alpha2);

        assert(std::numeric_limits<uint16_t>::max() > iso3166_1stringTable.size());
        alpha2NameMap.push_back({alpha2Key, (uint16_t)iso3166_1stringTable.size()});
        iso3166_1stringTable.append(entry.value(QLatin1String("name")).toString().toUtf8());
        iso3166_1stringTable.append('\0');

        const auto alpha3Key = IsoCodes::alpha3CodeToKey(entry.value(QLatin1String("alpha_3")).toString());
        alpha3alpha2Map.push_back({alpha3Key, alpha2Key});
    }

    std::sort(alpha2NameMap.begin(), alpha2NameMap.end());
    std::sort(alpha3alpha2Map.begin(), alpha3alpha2Map.end());

    // write out binary cache file
    QDir().mkpath(cachePath());
    QFile cache(cacheFilePath(u"iso_3166-1"));
    if (!cache.open(QFile::WriteOnly)) {
        qCWarning(KI18N) << "Failed to write ISO 3166-1 cache:" << cache.errorString() << cache.fileName();
        return;
    }

    uint32_t n = Iso3166_1CacheHeader;
    cache.write(reinterpret_cast<const char *>(&n), 4); // header
    n = alpha2NameMap.size();
    cache.write(reinterpret_cast<const char *>(&n), 4); // size
    for (auto entry : alpha2NameMap) {
        cache.write(reinterpret_cast<const char *>(&entry), sizeof(entry));
    }
    for (auto entry : alpha3alpha2Map) {
        cache.write(reinterpret_cast<const char *>(&entry), sizeof(entry));
    }
    cache.write(iso3166_1stringTable);
}

void IsoCodesCache::loadIso3166_2()
{
    if (!m_iso3166_2CacheData && !loadIso3166_2Cache()) {
        createIso3166_2Cache();
        loadIso3166_2Cache();
    }
}

bool IsoCodesCache::loadIso3166_2Cache()
{
    QFileInfo jsonFi(isoCodesPath(u"iso_3166-2.json"));
    auto f = std::make_unique<QFile>(cacheFilePath(u"iso_3166-2"));
    if (!f->open(QFile::ReadOnly) || f->fileTime(QFile::FileModificationTime) < jsonFi.lastModified() || f->size() < 8) {
        return false;
    }

    // validate cache file is usable
    // header matches
    const auto data = f->map(0, f->size());
    if (*reinterpret_cast<const uint32_t *>(data) != Iso3166_2CacheHeader) {
        return false;
    }
    // name lookup table fits into the available size
    auto size = *(reinterpret_cast<const uint32_t *>(data) + 1);
    auto offset = 3 * sizeof(uint32_t) + size * sizeof(MapEntry<uint32_t>);
    if (offset >= (std::size_t)f->size()) {
        return false;
    }
    // hierarchy map boundary check
    size = *(reinterpret_cast<const uint32_t *>(data + offset) - 1);
    offset += size * sizeof(MapEntry<uint32_t>);
    if (offset >= (std::size_t)f->size()) {
        return false;
    }
    // string table is 0 terminated
    if (data[f->size() - 1] != '\0') {
        return false;
    }

    m_iso3166_2CacheFile = std::move(f);
    m_iso3166_2CacheData = data;
    return true;
}

uint32_t IsoCodesCache::subdivisionCount() const
{
    return m_iso3166_2CacheData ? *(reinterpret_cast<const uint32_t *>(m_iso3166_2CacheData) + 1) : 0;
}

const MapEntry<uint32_t> *IsoCodesCache::subdivisionNameMapBegin() const
{
    return m_iso3166_2CacheData ? reinterpret_cast<const MapEntry<uint32_t> *>(m_iso3166_2CacheData + 2 * sizeof(uint32_t)) : nullptr;
}

uint32_t IsoCodesCache::subdivisionHierachyMapSize() const
{
    return m_iso3166_2CacheData
        ? *(reinterpret_cast<const uint32_t *>(m_iso3166_2CacheData + 2 * sizeof(uint32_t) + subdivisionCount() * sizeof(MapEntry<uint32_t>)))
        : 0;
}

const MapEntry<uint32_t> *IsoCodesCache::subdivisionParentMapBegin() const
{
    return m_iso3166_2CacheData
        ? reinterpret_cast<const MapEntry<uint32_t> *>(m_iso3166_2CacheData + 3 * sizeof(uint32_t) + subdivisionCount() * sizeof(MapEntry<uint32_t>))
        : nullptr;
}

const char *IsoCodesCache::subdivisionStringTableLookup(uint16_t offset) const
{
    if (m_iso3166_2CacheData) {
        offset += 3 * sizeof(uint32_t) + (subdivisionCount() + subdivisionHierachyMapSize()) * sizeof(MapEntry<uint32_t>);
        return m_iso3166_2CacheFile->size() > offset ? reinterpret_cast<const char *>(m_iso3166_2CacheData + offset) : nullptr;
    }
    return nullptr;
}

void IsoCodesCache::createIso3166_2Cache()
{
    qCDebug(KI18N) << "Rebuilding ISO 3166-2 cache";
    const auto path = isoCodesPath(u"iso_3166-2.json");

    QFile file(path);
    if (!file.open(QFile::ReadOnly)) {
        qCWarning(KI18N) << "Unable to open iso_3166-2.json" << path << file.errorString();
        return;
    }

    std::vector<MapEntry<uint32_t>> subdivNameMap;
    std::vector<MapEntry<uint32_t>> subdivParentMap;
    QByteArray iso3166_2stringTable;

    const auto doc = QJsonDocument::fromJson(file.readAll());
    const auto array = doc.object().value(QLatin1String("3166-2")).toArray();
    for (const auto &entryVal : array) {
        const auto entry = entryVal.toObject();
        const auto key = IsoCodes::subdivisionCodeToKey(entry.value(QLatin1String("code")).toString());

        assert(std::numeric_limits<uint16_t>::max() > iso3166_2stringTable.size());
        subdivNameMap.push_back({key, (uint16_t)iso3166_2stringTable.size()});
        iso3166_2stringTable.append(entry.value(QLatin1String("name")).toString().toUtf8());
        iso3166_2stringTable.append('\0');

        const auto parentKey = IsoCodes::alphaNum3CodeToKey(entry.value(QLatin1String("parent")).toString());
        if (parentKey) {
            subdivParentMap.push_back({key, parentKey});
        }
    }

    std::sort(subdivNameMap.begin(), subdivNameMap.end());
    std::sort(subdivParentMap.begin(), subdivParentMap.end());

    // write out binary cache file
    QDir().mkpath(cachePath());
    QFile cache(cacheFilePath(u"iso_3166-2"));
    if (!cache.open(QFile::WriteOnly)) {
        qCWarning(KI18N) << "Failed to write ISO 3166-2 cache:" << cache.errorString() << cache.fileName();
        return;
    }

    uint32_t n = Iso3166_2CacheHeader;
    cache.write(reinterpret_cast<const char *>(&n), 4); // header
    n = subdivNameMap.size();
    cache.write(reinterpret_cast<const char *>(&n), 4); // size of the name map
    for (auto entry : subdivNameMap) {
        cache.write(reinterpret_cast<const char *>(&entry), sizeof(entry));
    }
    n = subdivParentMap.size();
    cache.write(reinterpret_cast<const char *>(&n), 4); // size of the hierarchy map
    for (auto entry : subdivParentMap) {
        cache.write(reinterpret_cast<const char *>(&entry), sizeof(entry));
    }
    cache.write(iso3166_2stringTable);
}
