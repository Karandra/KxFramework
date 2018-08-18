#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxINetEvent.h"
class KxINet;
class KxINetConnection;

class KxINetRequest
{
	friend class KxINetConnection;

	private:
		using HINTERNET = void*;
		using HeadersMap = std::unordered_map<wxString, wxString>;
		using URLItemsArray = std::vector<std::pair<wxString, wxString>>;

	private:
		HeadersMap m_HeadersTable;
		URLItemsArray m_URLItems;
		wxUniChar m_ValueDelimiter = '=';
		wxUniChar m_PairDelimiter = '&';

	private:
		const URLItemsArray& GetURLItems() const
		{
			return m_URLItems;
		}
		const HeadersMap& GetHeaders() const
		{
			return m_HeadersTable;
		}
		wxString ConstructFullURL(const wxString& baseURL) const;

	public:
		void AddQueryItem(const wxString& name, const wxString& value)
		{
			m_URLItems.push_back(std::make_pair(name, value));
		}

		void SetHeader(const wxString& name, const wxString& value)
		{
			m_HeadersTable[name] = value;
		}
		wxString GetHeader(const wxString& name) const
		{
			auto it = m_HeadersTable.find(name);
			if (it != m_HeadersTable.end())
			{
				return m_HeadersTable.at(name);
			}
			return wxEmptyString;
		}

		void Clear()
		{
			m_HeadersTable.clear();
			m_URLItems.clear();
		}
		
		wxUniChar GetValueDelimiter() const
		{
			return m_ValueDelimiter;
		}
		wxUniChar GetPairDelimiter() const
		{
			return m_PairDelimiter;
		}
		void SetQueryDelimiters(const wxUniChar& valueDelimiter, const wxUniChar& pairDelimiter)
		{
			m_ValueDelimiter = valueDelimiter;
			m_PairDelimiter = pairDelimiter;
		}
};
