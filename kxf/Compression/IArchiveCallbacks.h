#pragma once
#include "Common.h"
#include "kxf/RTTI/QueryInterface.h"

namespace kxf
{
	class IArchiveExtract;
	class IArchiveUpdate;
}

namespace kxf::Compression
{
	class KX_API IExtractCallback: public RTTI::Interface<IExtractCallback>
	{
		KxDeclareIID(IExtractCallback, {0x8a6363c5, 0x35be, 0x4884, {0x8a, 0x35, 0x5e, 0x14, 0x5, 0x81, 0xbc, 0x25}});

		public:
			virtual ~IExtractCallback() = default;

		public:
			virtual bool ShouldCancel() const
			{
				return false;
			}

			virtual OutputStreamDelegate OnGetStream(const FileItem& item) = 0;
			virtual bool OnItemDone(const FileItem& item, wxOutputStream& stream) = 0;
	};

	template<class TOutStream = wxOutputStream>
	class KX_API ExtractWithOptions: public IExtractCallback
	{
		private:
			const IArchiveExtract& m_Archive;

			std::function<bool()> m_ShouldCancel;
			std::function<OutputStreamDelegate(const FileItem&)> m_OnGetStream;
			std::function<bool(const FileItem&, wxOutputStream&)> m_OnOperationCompleted;

		private:
			bool ShouldCancel() const override
			{
				return m_ShouldCancel ? std::invoke(m_ShouldCancel) : false;
			}

			OutputStreamDelegate OnGetStream(const FileItem& item) override
			{
				return m_OnGetStream ? std::invoke(m_OnGetStream, item) : nullptr;
			}
			bool OnItemDone(const FileItem& item, wxOutputStream& stream) override
			{
				return m_OnOperationCompleted ? std::invoke(m_OnOperationCompleted, item, stream) : true;
			}
			
		public:
			ExtractWithOptions(const IArchiveExtract& archive)
				:m_Archive(archive)
			{
				static_assert(std::is_base_of_v<wxOutputStream, TOutStream>, "invalid stream type");
			}

		public:
			bool Execute()
			{
				return m_Archive.Extract(*this);
			}
			bool Execute(Compression::FileIndexView files)
			{
				return m_Archive.Extract(*this, files);
			}
			
			template<class TFunc>
			ExtractWithOptions& ShouldCancel(TFunc&& func)
			{
				m_ShouldCancel = std::forward<TFunc>(func);
				return *this;
			}

			template<class TFunc>
			ExtractWithOptions& OnGetStream(TFunc&& func)
			{
				m_OnGetStream = std::forward<TFunc>(func);
				return *this;
			}

			template<class TFunc>
			ExtractWithOptions& OnItemDone(TFunc&& func)
			{
				if constexpr(std::is_same_v<TOutStream, wxOutputStream>)
				{
					// Assign as is
					m_OnOperationCompleted = std::forward<TFunc>(func);
				}
				else
				{
					// Wrap inside lambda and cast stream type
					m_OnOperationCompleted = [func = std::forward<TFunc>(func)](size_t index, wxOutputStream& stream) -> bool
					{
						return std::invoke(func, index, static_cast<TOutStream&>(stream));
					};
				}
				return *this;
			}
	};
}

namespace kxf::Compression
{
	class KX_API IUpdateCallback: public RTTI::Interface<IUpdateCallback>
	{
		KxDeclareIID(IUpdateCallback, {0xab48769d, 0x339b, 0x4579, {0xbd, 0x55, 0xbc, 0x51, 0xa6, 0x93, 0xdc, 0xaa}});

		public:
			virtual ~IUpdateCallback() = default;

		public:
			virtual bool ShouldCancel() const
			{
				return false;
			}

			virtual size_t OnGetUpdateMode(size_t index, bool& updateData, bool& updateProperties) = 0;
			virtual FileItem OnGetProperties(size_t index) = 0;

			virtual InputStreamDelegate OnGetStream(const FileItem& item) = 0;
			virtual bool OnItemDone(const FileItem& item, wxInputStream& stream) = 0;
	};
}
