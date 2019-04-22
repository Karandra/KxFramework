#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxUtility.h"
#include "Common.h"
#include "Row.h"
class wxDragImage;
class KX_API KxSplashWindow;

namespace KxDataView2
{
	class KX_API MainWindow;
}

namespace KxDataView2
{
	class DNDOperationInfo
	{
		private:
			wxDataObjectSimple* m_DropDataObject = nullptr;
			DNDOpType m_Type = DNDOpType::None;
			bool m_IsPreferedDrop = false;

		public:
			DNDOperationInfo() = default;
			DNDOperationInfo(wxDataObjectSimple& dataObject, DNDOpType type, bool isPreferredDrop = false)
				:m_DropDataObject(&dataObject), m_Type(type), m_IsPreferedDrop(isPreferredDrop)
			{
			}

		public:
			bool IsOK() const
			{
				return m_DropDataObject && m_Type != DNDOpType::None;
			}
			DNDOpType GetType() const
			{
				return m_Type;
			}
			wxDataObjectSimple& GetDataObject() const
			{
				return *m_DropDataObject;
			}

			void Combine(const DNDOperationInfo& other)
			{
				m_Type = (DNDOpType)(m_Type|other.m_Type);
				m_DropDataObject = other.m_DropDataObject;
				m_IsPreferedDrop = other.m_IsPreferedDrop;
			}
	};
}

namespace KxDataView2
{
	class KX_API DnDInfo
	{
		public:
			enum class Result
			{
				None,
				OperationAdded,
				OperationChanged,
				OperationRemoved,
			};

		private:
			std::map<wxDataFormat, DNDOperationInfo> m_DataFormats;

		private:
			bool DoCheckOperation(const wxDataFormat& format, DNDOpType desiredType) const;
			Result DoChangeOperation(const wxDataFormat& format, const DNDOperationInfo& info);

		public:
			DNDOperationInfo* GetOperationInfo(const wxDataFormat& format)
			{
				if (auto it = m_DataFormats.find(format); it != m_DataFormats.end())
				{
					return &it->second;
				}
				return nullptr;
			}
			const DNDOperationInfo* GetOperationInfo(const wxDataFormat& format) const
			{
				return const_cast<DnDInfo*>(this)->GetOperationInfo(format);
			}

			bool IsDragEnabled(const wxDataFormat& format) const
			{
				return DoCheckOperation(format, DNDOpType::Drag);
			}
			bool IsDropEnabled(const wxDataFormat& format) const
			{
				return DoCheckOperation(format, DNDOpType::Drop);
			}
			bool IsFullEnabled(const wxDataFormat& format) const
			{
				return DoCheckOperation(format, DNDOpType::Drag|DNDOpType::Drop);
			}

			Result EnableOperation(wxDataObjectSimple& dataObject, DNDOpType type, bool isPreferredDrop = false)
			{
				return DoChangeOperation(dataObject.GetFormat(), DNDOperationInfo(dataObject, type, isPreferredDrop));
			}
			Result DisableOperations(const wxDataFormat& format)
			{
				return DoChangeOperation(format, {});
			}
	};
}

namespace KxDataView2
{
	class KX_API DropSource: public wxDropSource
	{
		private:
			MainWindow* m_MainWindow = nullptr;
			KxSplashWindow* m_DragImage = nullptr;
			wxBitmap m_HintBitmap;
			wxPoint m_HintPosition;

			wxPoint m_Distance = wxDefaultPosition;
			Row m_Row;

		private:
			void OnScroll(wxMouseEvent& event);
			virtual bool GiveFeedback(wxDragResult effect) override;

			wxPoint GetHintPosition(const wxPoint& mousePos) const;

		public:
			DropSource(MainWindow* mainWindow, Row row);
			virtual ~DropSource();
	};
}

namespace KxDataView2
{
	class KX_API DropTarget: public wxDropTarget
	{
		private:
			MainWindow* m_MainWindow = nullptr;

		protected:
			wxDataFormat GetReceivedFormat() const;
			bool IsFormatSupported(const wxDataFormat& format, wxDataObjectSimple*& dataObject) const;

			virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult dragResult) override;
			virtual bool OnDrop(wxCoord x, wxCoord y) override;
			virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult dragResult) override;

			virtual wxDragResult OnEnter(wxCoord x, wxCoord y, wxDragResult dragResult) override;
			virtual void OnLeave() override;

		public:
			DropTarget(wxDataObject* dataObject, MainWindow* mainWindow);
	};
}
