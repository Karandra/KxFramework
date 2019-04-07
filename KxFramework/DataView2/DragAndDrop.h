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
	class KX_API DnDInfo
	{
		private:
			enum class OperationType
			{
				None = 0,
				Drag = 1 << 0,
				Drop = 1 << 1,

				All = Drag|Drop,
			};
			class Operation
			{
				private:
					wxDataObjectSimple* m_DropDataObject = nullptr;
					OperationType m_Type = OperationType::None;
					bool m_IsPreferedDrop = false;

				public:
					Operation() = default;
					Operation(OperationType type)
						:m_Type(type)
					{
					}
					Operation(wxDataObjectSimple& dataObject, bool isPreferredDrop = false)
						:m_Type(OperationType::Drop), m_DropDataObject(&dataObject), m_IsPreferedDrop(isPreferredDrop)
					{
					}

				public:
					bool IsOK() const
					{
						if (KxUtility::HasFlag(m_Type, OperationType::Drop))
						{
							return m_DropDataObject != nullptr;
						}
						return m_Type != OperationType::None;
					}
					OperationType GetType() const
					{
						return m_Type;
					}
					
					void Combine(const Operation& other)
					{
						m_Type = (OperationType)((int)m_Type|(int)other.m_Type);
						m_DropDataObject = other.m_DropDataObject;
						m_IsPreferedDrop = other.m_IsPreferedDrop;
					}
			};

		public:
			enum class Result
			{
				None,
				OperationAdded,
				OperationChanged,
				OperationRemoved,
			};

		private:
			std::map<wxDataFormat, Operation> m_DataFormats;

		private:
			Operation* FindFormat(const wxDataFormat& format)
			{
				if (auto it = m_DataFormats.find(format); it != m_DataFormats.end())
				{
					return &it->second;
				}
				return nullptr;
			}
			const Operation* FindFormat(const wxDataFormat& format) const
			{
				return const_cast<DnDInfo*>(this)->FindFormat(format);
			}
			
			bool IsOperationEnabled(const wxDataFormat& format, OperationType desiredType) const;
			Result EnableOperation(const wxDataFormat& format, const Operation& operation);

		public:
			bool IsDragEnabled(const wxDataFormat& format) const
			{
				return IsOperationEnabled(format, OperationType::Drag);
			}
			bool IsDropEnabled(const wxDataFormat& format) const
			{
				return IsOperationEnabled(format, OperationType::Drop);
			}
			bool IsAllEnabled(const wxDataFormat& format) const
			{
				return IsOperationEnabled(format, OperationType::All);
			}

			Result EnableDragOperation(const wxDataFormat& format)
			{
				return EnableOperation(format, Operation(OperationType::Drag));
			}
			Result EnableDropOperation(wxDataObjectSimple& dataObject, bool isPreferred = false)
			{
				return EnableOperation(dataObject.GetFormat(), Operation(dataObject, isPreferred));
			}
			Result EnableAllOperations(wxDataObjectSimple& dataObject, bool isPreferredDrop = false)
			{
				return EnableOperation(dataObject.GetFormat(), Operation(dataObject, isPreferredDrop));
			}
			Result DisableOperations(const wxDataFormat& format)
			{
				return EnableOperation(format, {});
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
