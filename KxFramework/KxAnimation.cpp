#include "KxStdAfx.h"
#include "KxFramework/KxAnimation.h"
#include "KxFramework/KxFileStream.h"

void KxSimpleAnimation::CopyWxAnimation(const wxAnimation& other)
{
	*static_cast<KxAnimation*>(this) = other;

	// Copy frames
	const size_t framesCount = other.GetFrameCount();
	m_Frames.clear();
	m_Frames.reserve(framesCount);

	for (size_t i = 0; i < framesCount; i++)
	{
		Add(other.GetFrame(i), other.GetDelay(i));
	}
}

bool KxSimpleAnimation::IsOk() const
{
	return KxAnimation::IsOk() && !m_Frames.empty();
}
int KxSimpleAnimation::GetDelay(unsigned int frameIndex) const
{
	if (frameIndex < m_Frames.size())
	{
		return m_Frames[frameIndex].m_Delay;
	}
	return 0;
}
wxImage KxSimpleAnimation::GetFrame(unsigned int frameIndex) const
{
	if (frameIndex < m_Frames.size())
	{
		return m_Frames[frameIndex].m_Bitmap.ConvertToImage();
	}
	return wxNullImage;
}
wxBitmap KxSimpleAnimation::GetBitmap(unsigned int frameIndex) const
{
	if (frameIndex < m_Frames.size())
	{
		return m_Frames[frameIndex].m_Bitmap;
	}
	return wxNullBitmap;
}
unsigned int KxSimpleAnimation::GetFrameCount() const
{
	return m_Frames.size();
}
wxSize KxSimpleAnimation::GetSize() const
{
	if (!m_Frames.empty())
	{
		return m_Frames.front().m_Bitmap.GetSize();
	}
	return wxDefaultSize;
}

void KxSimpleAnimation::Clear()
{
	ClearFrames();
}
void KxSimpleAnimation::Add(const wxBitmap& bitmap, int delay)
{
	m_Frames.emplace_back(bitmap, delay);
}
void KxSimpleAnimation::Add(const wxImage& bitmap, int delay)
{
	m_Frames.emplace_back(wxBitmap(bitmap, BITMAP_DEPTH), delay);
}
bool KxSimpleAnimation::LoadAdd(wxInputStream& stream, int delay)
{
	if (stream.IsOk())
	{
		Frame& frame = m_Frames.emplace_back(wxBitmap(wxImage(stream, wxBITMAP_TYPE_ANY), BITMAP_DEPTH), delay);
		return frame.m_Bitmap.IsOk();
	}
	return false;
}
bool KxSimpleAnimation::LoadAddFile(const wxString& path, int delay)
{
	KxFileStream stream(path, KxFileStream::Access::Read, KxFileStream::Disposition::OpenExisting);
	return LoadAdd(stream, delay);
}
