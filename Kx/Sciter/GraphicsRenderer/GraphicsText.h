#pragma once
#include "Kx/Sciter/Common.h"
#include "Kx/Sciter/Utility/HandleWrapper.h"

namespace KxFramework::Sciter
{
	struct GraphicsTextHandle;
	class ScriptValue;
	class Element;
}

namespace KxFramework::Sciter
{
	class KX_API GraphicsText final: public HandleWrapper<GraphicsText, GraphicsTextHandle>
	{
		friend class HandleWrapper<GraphicsText, GraphicsTextHandle>;
		
		private:
			GraphicsTextHandle* m_Handle = nullptr;

		private:
			bool DoAcquire(GraphicsTextHandle* handle);
			void DoRelease();

		public:
			GraphicsText() = default;
			GraphicsText(GraphicsTextHandle* handle)
				:HandleWrapper(handle)
			{
			}
			GraphicsText(const GraphicsText& other)
				:HandleWrapper(other)
			{
			}
			GraphicsText(GraphicsText&& other)
				:HandleWrapper(std::move(other))
			{
			}
			GraphicsText(const ScriptValue& value);

		public:
			ScriptValue ToScriptValue() const;

			bool CreateForElement(const wxString& text, const Element& element, const wxString& className = {});
			bool CreateForElementAndStyle(const wxString& text, const Element& element, const wxString& style = {});
			
			std::optional<TextMetrics> GetMetrics() const;
			wxSize GetMinExtent() const
			{
				auto metrics = GetMetrics();
				return metrics ? wxSize(metrics->MinWidth, metrics->Height) : wxDefaultSize;
			}
			wxSize GetMaxExtent() const
			{
				auto metrics = GetMetrics();
				return metrics ? wxSize(metrics->MaxWidth, metrics->Height) : wxDefaultSize;
			}
			int GetLineCount() const
			{
				auto metrics = GetMetrics();
				return metrics ? metrics->LineCount : 0;
			}

			bool SetBox(double width, double height);
			bool SetBox(const wxSize& box)
			{
				return SetBox(box.GetWidth(), box.GetHeight());
			}

		public:
			GraphicsText& operator=(const GraphicsText& other)
			{
				CopyFrom(other);
				return *this;
			}
			GraphicsText& operator=(GraphicsText&& other)
			{
				MoveFrom(other);
				return *this;
			}
			GraphicsText& operator=(GraphicsTextHandle* handle)
			{
				CopyFrom(handle);
				return *this;
			}
	};
}
