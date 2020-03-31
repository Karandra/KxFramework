#include "KxStdAfx.h"
#include "ShellOperations.h"
#include "Kx/Utility/Common.h"
#include "Kx/Utility/CallAtScopeExit.h"

namespace KxFramework::Shell
{
	bool FileOperation(SHOperationType opType, const FSPath& source, const FSPath& destination, wxWindow* window, SHOperationFlags flags)
	{
		// SHFileOperation doesn't work for paths longer than 'MAX_PATH'
		if (source.GetPathLength() >= MAX_PATH || destination.GetPathLength() >= MAX_PATH)
		{
			return false;
		}

		SHFILEOPSTRUCTW operationInfo = {};

		// Set operation
		switch (opType)
		{
			case SHOperationType::Copy:
			{
				operationInfo.wFunc = FO_COPY;
				break;
			}
			case SHOperationType::Move:
			{
				operationInfo.wFunc = FO_MOVE;
				break;
			}
			case SHOperationType::Rename:
			{
				operationInfo.wFunc = FO_RENAME;
				break;
			}
			case SHOperationType::Delete:
			{
				operationInfo.wFunc = FO_DELETE;
				break;
			}
			default:
			{
				return false;
			}
		};

		// Parent window
		window = window ? wxGetTopLevelParent(window) : nullptr;
		operationInfo.hwnd = window ? window->GetHandle() : nullptr;

		// Set flags
		operationInfo.fFlags = FOF_NOCONFIRMMKDIR;
		Utility::ModFlagRef(operationInfo.fFlags, FOF_NO_UI, !operationInfo.hwnd);
		Utility::ModFlagRef(operationInfo.fFlags, FOF_FILESONLY|FOF_NORECURSION, flags & SHOperationFlags::LimitToFiles);
		Utility::ModFlagRef(operationInfo.fFlags, FOF_ALLOWUNDO, flags & SHOperationFlags::AllowUndo);
		Utility::ModFlagRef(operationInfo.fFlags, FOF_NOCONFIRMATION, flags & SHOperationFlags::NoConfirmation);
		Utility::ModFlagRef(operationInfo.fFlags, FOF_NORECURSION, !(flags & SHOperationFlags::Recursive));

		// Paths
		auto CreateZZString = [](const wxString& s) -> std::wstring
		{
			if (!s.IsEmpty())
			{
				std::wstring stringZZ;
				stringZZ.reserve(s.length() + 2);
				stringZZ.append(s.wx_str(), s.length());
				stringZZ.append(2, L'\0');

				return stringZZ;
			}
			return {};
		};
		std::wstring sourceZZ = CreateZZString(source.GetFullPath());
		std::wstring destinationZZ = CreateZZString(destination.GetFullPath());

		operationInfo.pFrom = sourceZZ.data();
		operationInfo.pTo = destinationZZ.data();

		// Disable parent window if UI actions is allowed
		bool disabled = false;
		if (window && window->IsThisEnabled())
		{
			window->Disable();
			disabled = true;
		}
		Utility::CallAtScopeExit atExit([&]()
		{
			// Re-enable parent window
			if (disabled)
			{
				window->Enable();
			}
		});

		// Perform the operation, zero means function succeeded.
		return ::SHFileOperationW(&operationInfo) == 0 && !operationInfo.fAnyOperationsAborted;
	}
}
