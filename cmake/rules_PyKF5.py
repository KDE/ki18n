#=============================================================================
# SPDX-FileCopyrightText: 2016 by Shaheed Haque (srhaque@theiet.org)
# SPDX-FileCopyrightText: 2016 Stephen Kelly <steveire@gmail.com>
#
# SPDX-License-Identifier: BSD-3-Clause
#=============================================================================

import os, sys

import rules_engine
sys.path.append(os.path.dirname(os.path.dirname(rules_engine.__file__)))
import Qt5Ruleset

from clang.cindex import CursorKind

def i18n_ellipsis(container, function, sip, matcher):
    if len(sip["template_parameters"]) == 0:
        sip["parameters"] += ["..."]
    else:
        sip["name"] = ""

def i18np_ellipsis(container, function, sip, matcher):
    if len(sip["template_parameters"]) == 1:
        sip["parameters"][-1] = "..."
        sip["template_parameters"] = ""
    else:
        sip["name"] = ""

def local_function_rules():
    return [
        ["KLocalizedString", "subs", ".*", ".*", ".*unsigned int.*", rules_engine.function_discard],
        ["KLocalizedString", "subs", ".*", ".*", ".*long.*", rules_engine.function_discard],
        ["KLocalizedString", "subs", ".*", ".*", ".*unsigned long.*", rules_engine.function_discard],
        ["KLocalizedString", "subs", ".*", ".*", ".*unsigned long long.*", rules_engine.function_discard],
        ["KLocalizedString", "subs", ".*", ".*", ".*QChar.*", rules_engine.function_discard],
        ["Kuit", "setupForDomain", ".*", ".*", ".*", rules_engine.function_discard],
        ["KuitSetup", "setTagPattern", ".*", ".*", ".*", rules_engine.function_discard],

        ["klocalizedstring.h", "i18n", ".*", ".*", ".*", i18n_ellipsis],
        ["klocalizedstring.h", "i18nc", ".*", ".*", ".*", i18n_ellipsis],
        ["klocalizedstring.h", "i18np", ".*", ".*", ".*", i18np_ellipsis],
        ["klocalizedstring.h", "i18ncp", ".*", ".*", ".*", i18np_ellipsis],
    ]

def local_typedef_rules():
    return [
        [".*", "TagFormatter", rules_engine.typedef_discard],
    ]

def _klocalizedstring_add_template_code(filename, sip, entry):
    sip["code"] = """
%ModuleCode
QString klocalizedstring_i18n_template(KLocalizedString base, PyObject *list,int *sipIsErr) {
    KLocalizedString result = base;
    QString *arg;
    long long_arg;
    double double_arg;
    int iserr = 0;

    for (int i=0; i < PyTuple_Size(list); i++) {
        PyObject *pyarg = PyTuple_GET_ITEM (list, i);
#if PY_MAJOR_VERSION >= 3
        if (PyLong_Check(pyarg)) {
            long_arg = PyLong_AsLong(pyarg);
#else
        if (PyInt_Check(pyarg)) {
            long_arg = PyInt_AsLong(pyarg);
#endif
            if (long_arg==-1 && PyErr_Occurred()) {
                *sipIsErr = 1;
                return QString();
            }
            result = result.subs(long_arg);

#if PY_MAJOR_VERSION >= 3
        } else if (PyNumber_Check(pyarg)) {
            PyObject *long_py = PyNumber_Long(pyarg);
            long_arg = PyLong_AsLong(long_py);
            Py_DECREF(long_py);
#else
        } else if (PyLong_Check(pyarg)) {
            long_arg = PyLong_AsLong(pyarg);
#endif
            if (long_arg==-1 && PyErr_Occurred()) {
                *sipIsErr = 1;
                return QString();
            }
            result = result.subs(long_arg);

        } else if (PyFloat_Check(pyarg)) {
            double_arg = PyFloat_AsDouble(pyarg);
            result = result.subs(double_arg);

        } else {
          int state = 0;
          arg = (QString *)sipForceConvertToType(pyarg, sipType_QString, NULL, SIP_NOT_NONE, &state, &iserr);
          if (iserr) {
              *sipIsErr = 1;
              return QString();
          }

          result = result.subs(*arg);
          sipReleaseType(arg,sipType_QString,state);
          arg = 0;
          }
    }

    return result.toString();
}
%End\n
"""

class RuleSet(Qt5Ruleset.RuleSet):
    """
    SIP file generator rules. This is a set of (short, non-public) functions
    and regular expression-based matching rules.
    """
    def __init__(self):
        Qt5Ruleset.RuleSet.__init__(self)
        self._fn_db = rules_engine.FunctionRuleDb(lambda: local_function_rules() + Qt5Ruleset.function_rules())
        self._typedef_db = rules_engine.TypedefRuleDb(lambda: local_typedef_rules())
        self._methodcode = rules_engine.MethodCodeDb({
            "klocalizedstring.h": {
                "i18n":
                {
                    "code":
                    """
                    %MethodCode
                        QString result = klocalizedstring_i18n_template(ki18n(a0),a1,&sipIsErr);
                        if (!sipIsErr) {
                            sipRes = new QString(result);
                        }
                    %End
                    """
                },
                "i18nc":
                {
                    "code":
                    """
                    %MethodCode
                        QString result = klocalizedstring_i18n_template(ki18nc(a0,a1),a2,&sipIsErr);
                        if (!sipIsErr) {
                            sipRes = new QString(result);
                        }
                    %End
                    """
                },
                "i18np":
                {
                    "code":
                    """
                    %MethodCode
                        QString result = klocalizedstring_i18n_template(ki18np(a0,a1),a2,&sipIsErr);
                        if (!sipIsErr) {
                            sipRes = new QString(result);
                        }
                    %End
                    """
                },
                "i18ncp":
                {
                    "code":
                    """
                    %MethodCode
                        QString result = klocalizedstring_i18n_template(ki18ncp(a0,a1,a2),a3,&sipIsErr);
                        if (!sipIsErr) {
                            sipRes = new QString(result);
                        }
                    %End
                    """
                },
            }
        })

        self._modulecode = rules_engine.ModuleCodeDb({
            "klocalizedstring.h":
            {
                "code": _klocalizedstring_add_template_code,
            },
            })
