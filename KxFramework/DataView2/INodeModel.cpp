#include "KxStdAfx.h"
#include "INodeModel.h"
#include "Editor.h"
#include "Renderer.h"

namespace KxDataView2
{
	bool INodeModel::IsEditable(const Column& column) const
	{
		return GetEditor(column) != nullptr;
	}
	bool INodeModel::IsActivatable(const Column& column) const
	{
		return GetRenderer(column).IsActivatable();
	}
}
