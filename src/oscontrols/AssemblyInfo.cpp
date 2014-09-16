#include "Version.h"

#define WIDEN(x) L#x

using namespace System;
using namespace System::Reflection;
using namespace System::Runtime::CompilerServices;
using namespace System::Runtime::InteropServices;
using namespace System::Security::Permissions;

//
// General Information about an assembly is controlled through the following
// set of attributes. Change these attribute values to modify the information
// associated with an assembly.
//
[assembly:AssemblyTitleAttribute(WIDEN(oscontrols_APPNAME))];
[assembly:AssemblyDescriptionAttribute(WIDEN(oscontrols_APPDESC))];
[assembly:AssemblyConfigurationAttribute(L"")];
[assembly:AssemblyCompanyAttribute(WIDEN(LEAP_COMPANY_NAME))];
[assembly:AssemblyProductAttribute(WIDEN(oscontrols_APPNAME))];
[assembly:AssemblyCopyrightAttribute(WIDEN(APP_COPYRIGHT))];
[assembly:AssemblyTrademarkAttribute(L"")];
[assembly:AssemblyCultureAttribute(L"")];

//
// Version information for an assembly consists of the following four values:
//
//      Major Version
//      Minor Version
//      Build Number
//      Revision
//
// You can specify all the value or you can default the Revision and Build Numbers
// by using the '*' as shown below:

[assembly:AssemblyVersionAttribute(oscontrols_WVERSION)];

[assembly:ComVisible(false)];

[assembly:CLSCompliantAttribute(true)];