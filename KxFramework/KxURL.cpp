#include "KxStdAfx.h"
#include "KxURL.h"

void KxURL::OnAssignAddress()
{
}

std::unique_ptr<wxInputStream> KxURL::GetInputStream()
{
	return std::unique_ptr<wxInputStream>(wxURL::GetInputStream());
}
