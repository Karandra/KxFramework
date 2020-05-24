#pragma once
#include "kxf/UI/Common.h"
#include "StdDialog.h"
#include "kxf/System/COM.h"
#include "kxf/System/ErrorCode.h"
#include "kxf/FileSystem/FSPath.h"
struct IFileDialog;
struct IFileDialog2;

namespace kxf::UI
{
	namespace Private
	{
		class FileBrowseDialogEvents;

		struct FilterItem final
		{
			String Name;
			String Filter;
		};
		struct FilterSpecItem final
		{
			const wchar_t* Name;
			const wchar_t* Filter;
		};
	}

	enum class FileBrowseDialogMode
	{
		Save,
		Open,
		OpenDirectory
	};
	enum class FileBrowseDialogStyle
	{
		None = 0,

		WarnCreate,
		WarnOverwrite,
		ForceFileSystem,
		ForceShowHidden,
		ForceExistingItem,
		ForcePreviewPane,
		SkipReadOnlyItems,
		AddToRecent,
		Multiselect,
		HidePinnedPlaces,
		NoDereferenceLinks,
		AllowStreamableItems,
	};
}
namespace kxf
{
	Kx_DeclareFlagSet(UI::FileBrowseDialogStyle);
}

namespace kxf::UI
{
	class KX_API FileBrowseDialog: public Dialog, public IStdDialog
	{
		friend class Private::FileBrowseDialogEvents;

		public:
			static constexpr  FlagSet<FileBrowseDialogStyle> DefaultStyle = FileBrowseDialogStyle::WarnOverwrite|FileBrowseDialogStyle::ForceFileSystem;

		private:
			COMPtr<IFileDialog> m_Instance;
			COMPtr<IFileDialog2> m_InstanceExtra;
			COMPtr<Private::FileBrowseDialogEvents> m_Events;

			void* m_Handle = nullptr;
			wxWindow* m_Parent = nullptr;
			HResult m_ShowStatus = HResult::Fail();
			FileBrowseDialogMode m_Mode = FileBrowseDialogMode::Open;
			FlagSet<FileBrowseDialogStyle> m_Style;
		
			String m_Caption;
			String m_Label;
			std::vector<Private::FilterItem> m_FilterList;
			std::vector<Private::FilterSpecItem> m_FilterListSpec;

		private:
			void ApplyFileFilters();

		public:
			FileBrowseDialog() = default;
			FileBrowseDialog(wxWindow* parent,
							 wxWindowID id,
							 FileBrowseDialogMode mode,
							 const String& caption = {},
							 FlagSet<StdButton> buttons = DefaultButtons,
							 FlagSet<FileBrowseDialogStyle> style = DefaultStyle

			)
			{
				Create(parent, id, mode, caption, buttons, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						FileBrowseDialogMode mode,
						const String& caption = {},
						FlagSet<StdButton> buttons = DefaultButtons,
						FlagSet<FileBrowseDialogStyle> style = DefaultStyle
			);
			~FileBrowseDialog();

		public:
			WXWidget GetHandle() const override
			{
				return reinterpret_cast<WXWidget>(m_Handle);
			}
			FileBrowseDialogMode GetMode() const
			{
				return m_Mode;
			}

			// Show and close the dialog
			int ShowModal() override;
			bool Show(bool show = true) override;
			bool Close(bool force = false);
			bool IsShown() const override;
			bool IsTopLevel() const override
			{
				return true;
			}

			// Window title
			wxString GetTitle() const override
			{
				return m_Caption;
			}
			void SetTitle(const wxString& title) override
			{
				SetCaption(title);
			}
			
			wxString GetCaption() const override
			{
				return GetTitle();
			}
			void SetCaption(const wxString& caption) override;
			
			wxString GetLabel() const override
			{
				return m_Label;
			}
			void SetLabel(const wxString& label) override;

			// Icons
			wxBitmap GetMainIcon() const override
			{
				return GetIcon();
			}
			StdIcon GetMainIconID() const override
			{
				return StdIcon::None;
			}
			void SetMainIcon(const wxBitmap& icon) override
			{
				wxIcon temp;
				temp.CopyFromBitmap(icon);
				SetIcon(temp);
			}
			void SetMainIcon(StdIcon iconID = DefaultIconID) override
			{
			}

			// Other options
			bool SetDirectory(const FSPath& path);
			bool SetNavigationRoot(const FSPath& path);
			bool SetButtonLabel(StdID buttonID, const String& text);
			
			String GetItemName() const;
			void SetItemName(const String& name);
			void SetDefaultExtension(const String& extension);
			
			size_t GetSelectedFilter() const;
			void SetSelectedFilter(size_t filterIndex);

			// Pinned places and filters
			bool AddPinnedPlace(const FSPath& path, const String& label = {}, bool top = true);
			size_t AddFilter(const String& filter, const String& label = {});
			size_t AddUnivsersalFilter();

			// Results
			FSPath GetResult() const;
			size_t EnumResults(std::function<bool(FSPath)> func) const;

			// Buttons
			void SetDefaultButton(wxWindowID id) override
			{
			}
			StdDialogControl GetButton(wxWindowID id) const override
			{
				if (m_Handle && (id == wxID_OK || id == wxID_CANCEL))
				{
					return id;
				}
				return wxID_NONE;
			}
			StdDialogControl AddButton(wxWindowID id, const String& label, bool prepend) override
			{
				return wxID_NONE;
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(FileBrowseDialog);
	};
}
