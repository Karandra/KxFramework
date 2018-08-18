#pragma once
#include "KxFramework/KxFramework.h"

class KxSecretValue
{
	private:
		KxUInt8Vector m_Storage;

	public:
		KxSecretValue()
		{
		}
		KxSecretValue(void* data, size_t size)
			:m_Storage(size, 0)
		{
			std::memcpy(m_Storage.data(), data, size);
		}
		KxSecretValue(const wxString& string)
		{
			auto utf8 = string.ToUTF8();

			m_Storage.resize(utf8.length(), 0);
			std::memcpy(m_Storage.data(), utf8.data(), utf8.length());
		}
		KxSecretValue(const KxSecretValue& other)
		{
			*this = other;
		}
		~KxSecretValue();

	public:
		bool IsOk() const
		{
			return !m_Storage.empty();
		}
		
		size_t GetSize() const
		{
			return m_Storage.size();
		}
		const void* GetData() const
		{
			return m_Storage.data();
		}
		wxString GetAsString(const wxMBConv& conv = wxConvWhateverWorks) const;
		void Wipe();

	public:
		KxSecretValue& operator=(const KxSecretValue& other)
		{
			const KxUInt8Vector& otherStore = other.m_Storage;
			m_Storage.assign(otherStore.begin(), otherStore.end());

			return *this;
		}
		bool operator==(const KxSecretValue& other) const
		{
			return m_Storage == other.m_Storage;
		}
		bool operator!=(const KxSecretValue& other) const
		{
			return !(*this == other);
		}
};

//////////////////////////////////////////////////////////////////////////
class KxSecretStore
{
	public:
		static KxSecretStore GetDefault();

	private:
		KxSecretStore();

	public:
		virtual ~KxSecretStore();

	public:
		virtual bool IsOk() const
		{
			return true;
		}

		virtual bool Save(const wxString& serviceName, const wxString& userName, const KxSecretValue& password);
		virtual bool Load(const wxString& serviceName, wxString& userName, KxSecretValue& password) const;
		virtual bool Delete(const wxString& serviceName);
};

//////////////////////////////////////////////////////////////////////////
class KxSecretStoreService
{
	private:
		const wxString m_ServiceName;

	public:
		KxSecretStoreService(const wxString& serviceName)
			:m_ServiceName(serviceName)
		{
		}

	public:
		bool IsOk() const
		{
			return !m_ServiceName.IsEmpty();
		}
		const wxString& GetServiceName() const
		{
			return m_ServiceName;
		}

		bool Save(KxSecretStore& store, const wxString& userName, const KxSecretValue& password)
		{
			return store.Save(m_ServiceName, userName, password);
		}
		bool Load(const KxSecretStore& store, wxString& userName, KxSecretValue& password) const
		{
			return store.Load(m_ServiceName, userName, password);
		}
		bool Delete(KxSecretStore& store)
		{
			return store.Delete(m_ServiceName);
		}
};

//////////////////////////////////////////////////////////////////////////
class KxSecretDefaultStoreService
{
	private:
		KxSecretStore m_Store;
		const wxString m_ServiceName;

	public:
		KxSecretDefaultStoreService(const wxString& serviceName)
			:m_Store(KxSecretStore::GetDefault()), m_ServiceName(serviceName)
		{
		}

	public:
		bool IsOk() const
		{
			return !m_ServiceName.IsEmpty();
		}
		const wxString& GetServiceName() const
		{
			return m_ServiceName;
		}

		bool Save(const wxString& userName, const KxSecretValue& password)
		{
			return m_Store.Save(m_ServiceName, userName, password);
		}
		bool Load(wxString& userName, KxSecretValue& password) const
		{
			return m_Store.Load(m_ServiceName, userName, password);
		}
		bool Delete()
		{
			return m_Store.Delete(m_ServiceName);
		}
};
