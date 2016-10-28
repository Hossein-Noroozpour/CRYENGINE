// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

#include "StdAfx.h"

#include "ItemSerializationSupport.h"

#include <yasli/JSONIArchive.h>
#include <yasli/JSONOArchive.h>

// *INDENT-OFF* - <hard to read code and declarations due to inconsistent indentation>

namespace uqs
{
	namespace core
	{

		class CUqsJsonIArchive : public yasli::JSONIArchive
		{
		public:
			CUqsJsonIArchive(int extraArchiveFlags)
			{
				caps_ |= extraArchiveFlags;
			}

			using JSONIArchive::operator();

			const char* GetErrorMessage() { return m_errorMessage.c_str(); }

		protected:
			virtual void validatorMessage(bool bError, const void* pHandle, const yasli::TypeID& type, const char* szMessage) override
			{
				if (!m_errorMessage.empty())
				{
					m_errorMessage += '\n';
				}
				m_errorMessage += szMessage;
			}

		private:
			stack_string m_errorMessage;
		};



		bool CItemSerializationSupport::DeserializeItemFromCStringLiteral(void* pOutItem, const client::IItemFactory& itemFactory, const char* szItemLiteral, shared::IUqsString* pErrorMessage) const
		{
			const bool bValidate = (pErrorMessage != nullptr);
			const int archiveFlags = bValidate ? CUqsJsonIArchive::VALIDATION : 0;
			bool bResult = false;
			CUqsJsonIArchive archive(archiveFlags);

			if (archive.open(szItemLiteral, strlen(szItemLiteral)))
			{
				bResult = itemFactory.TryDeserializeItem(pOutItem, archive, "value", "");
			}

			if (bValidate)
			{
				pErrorMessage->Set(archive.GetErrorMessage());
			}

			return bResult;
		}

		bool CItemSerializationSupport::DeserializeItemIntoDictFromCStringLiteral(shared::IVariantDict& out, const char* szKey, client::IItemFactory& itemFactory, const char* szItemLiteral, shared::IUqsString* pErrorMessage) const
		{
			const bool bValidate = (pErrorMessage != nullptr);
			const int archiveFlags = bValidate ? CUqsJsonIArchive::VALIDATION : 0;
			bool bResult = false;
			CUqsJsonIArchive archive(archiveFlags);

			if (archive.open(szItemLiteral, strlen(szItemLiteral)))
			{
				bResult = itemFactory.TryDeserializeItemIntoDict(out, szKey, archive, "value", "");
			}
			
			if (bValidate)
			{
				pErrorMessage->Set(archive.GetErrorMessage());
			}

			return bResult;
		}

		bool CItemSerializationSupport::SerializeItemToStringLiteral(const void* pItem, const client::IItemFactory& itemFactory, shared::IUqsString& outString) const
		{
			yasli::JSONOArchive archive;

			if (itemFactory.TrySerializeItem(pItem, archive, "value", ""))
			{
				const char* szResult = archive.c_str();
				
				// HACK - for some reason, JSONOArchive puts \n at first line, so let's skip it.
				if (szResult[0] == '\n')
				{
					++szResult;
				}

				outString.Set(szResult);
				return true;
			}

			return false;
		}
	}
}
