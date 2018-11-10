#pragma once
#include "KxFramework/KxFramework.h"
#include <wx/animate.h>

class KxAnimation: public wxAnimation
{
	public:
		enum
		{
			BITMAP_DEPTH = 32
		};

	public:
		KxAnimation() = default;
		KxAnimation(const wxAnimation& other)
			:wxAnimation(other)
		{
		}
		virtual ~KxAnimation() = default;

	public:
		virtual wxBitmap GetBitmap(unsigned int frameIndex) const
		{
			return wxBitmap(wxAnimation::GetFrame(frameIndex), BITMAP_DEPTH);
		}

	public:
		KxAnimation& operator=(const wxAnimation& other)
		{
			*static_cast<wxAnimation*>(this) = other;
			return *this;
		}
		
		bool operator==(const wxAnimation&) = delete;
		bool operator!=(const wxAnimation&) = delete;
};

//////////////////////////////////////////////////////////////////////////
class KxSimpleAnimation: public KxAnimation
{
	private:
		class Frame
		{
			friend class KxSimpleAnimation;

			private:
				wxBitmap m_Bitmap;
				int m_Delay = 0;

			public:
				Frame(const wxBitmap& bitmap, int delay)
					:m_Bitmap(bitmap), m_Delay(delay)
				{
				}
		};

	private:
		std::vector<Frame> m_Frames;

	protected:
		void ClearFrames()
		{
			m_Frames.clear();
		}
		void CopyWxAnimation(const wxAnimation& other);

	public:
		KxSimpleAnimation() = default;
		KxSimpleAnimation(const wxAnimation& other)
		{
			*this = other;
		}
		KxSimpleAnimation(const KxSimpleAnimation& other)
		{
			*this = other;
		}
		KxSimpleAnimation(KxSimpleAnimation&& other)
		{
			*this = std::move(other);
		}

	public:
		virtual bool IsOk() const override;
		virtual int GetDelay(unsigned int frameIndex) const override;
		virtual wxImage GetFrame(unsigned int frameIndex) const override;
		virtual wxBitmap GetBitmap(unsigned int frameIndex) const override;
		virtual unsigned int GetFrameCount() const override;
		virtual wxSize GetSize() const override;

		void Clear();
		void Add(const wxBitmap& bitmap, int delay);
		void Add(const wxImage& bitmap, int delay);
		bool LoadAdd(wxInputStream& stream, int delay);
		bool LoadAddFile(const wxString& path, int delay);

		virtual bool Load(wxInputStream& stream, wxAnimationType type = wxANIMATION_TYPE_ANY) override
		{
			return false;
		}
		virtual bool LoadFile(const wxString& path, wxAnimationType type = wxANIMATION_TYPE_ANY) override
		{
			return false;
		}

	public:
		KxSimpleAnimation& operator=(const wxAnimation& other)
		{
			CopyWxAnimation(other);
			return *this;
		}
		KxSimpleAnimation& operator=(const KxSimpleAnimation& other)
		{
			CopyWxAnimation(other);
			m_Frames = other.m_Frames;
			return *this;
		}
		KxSimpleAnimation& operator=(KxSimpleAnimation&& other)
		{
			CopyWxAnimation(other);
			m_Frames = std::move(other.m_Frames);
			return *this;
		}
};
