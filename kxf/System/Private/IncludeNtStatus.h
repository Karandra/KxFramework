// Save 'WIN32_NO_STATUS' macro value and undefine it to define complete set of 'NTSTATUS' values
#pragma push_macro("WIN32_NO_STATUS")
#undef WIN32_NO_STATUS

// Since some NTSTATUS'es might be already defined in other parts of Windows headers suppress the macro redefinition warning
#pragma warning(push, 0)
#pragma warning(disable: 4005)

#include <ntstatus.h>

// Restore macro value if any
#pragma pop_macro("WIN32_NO_STATUS")

// Restore any disabled warnings
#pragma warning(pop)
