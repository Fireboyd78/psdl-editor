#ifndef __DOCVIEW_H__
#define __DOCVIEW_H__

#include "io_error.h"
#include <string>

template <class TDoc>
class View
{
	public:
		virtual void SetDocument(TDoc* pDoc)
		{
			m_pDoc = pDoc;
		}

		virtual void RenderScene(HDC hDC, HGLRC hRC) {}

	protected:
		TDoc* m_pDoc;
};

class DocTemplateBase
{
public:

	virtual std::string GetFileName(bool bFull = false) = 0;
	virtual error::code OpenDocument(LPCTSTR lpszPathName) = 0;
	virtual error::code SaveDocument(LPCTSTR lpszPathName = NULL) = 0;
	virtual bool FileExists(void) = 0;
	virtual bool IsModified(void) = 0;
};

template <class TDoc, class TView>
class DocTemplate : public DocTemplateBase
{
public:

	DocTemplate(void)
	{
		m_pDoc = NULL;
		m_pView = new TView();
		m_bFileExists = false;
		m_bModified = false;
		m_sFileName = _T("");
	}

	virtual ~DocTemplate(void)
	{
		if (m_pDoc)
			delete m_pDoc;
		if (m_pView)
			delete m_pView;
	}

	TView* GetView(void)
	{
		return m_pView;
	}

	TDoc* GetDocument(void)
	{
		return m_pDoc;
	}

	std::string GetFileName(bool bFull = false)
	{
		if (!bFull)
		{
			size_t pos = m_sFileName.rfind(_T('\\'));
			
			if (pos == m_sFileName.npos)
				pos = m_sFileName.rfind(_T('/'));
			
			if (pos != m_sFileName.npos)
				return m_sFileName.substr(++pos);
		}

		return m_sFileName;
	}

	virtual void NewDocument(std::string sFileName = _T("untitled"))
	{
		if (m_pDoc) delete m_pDoc;
		m_pDoc = new TDoc();
		m_pView->SetDocument(m_pDoc);
		m_sFileName = sFileName;
		m_bModified = true;
		m_bFileExists = false;
		UpdateViews();
	}

	error::code OpenDocument(LPCTSTR lpszPathName)
	{
		TDoc* pDoc = new TDoc();

		error::code code = pDoc->read_file(lpszPathName);

		if (code == error::ok)
		{
			if (m_pDoc) delete m_pDoc;
			m_pDoc = pDoc;
			m_pView->SetDocument(m_pDoc);
			m_sFileName = lpszPathName;
			m_bModified = false;
			m_bFileExists = true;
			UpdateViews();
		}
		else
		{
			delete pDoc;
		}

		return code;
	}

	error::code SaveDocument(LPCTSTR lpszPathName = NULL)
	{
		if (!lpszPathName)
			lpszPathName = m_sFileName.c_str();

		error::code code = m_pDoc->write_file(lpszPathName);

		if (code == error::ok)
		{
			m_sFileName = lpszPathName;
			m_bModified = false;
			m_bFileExists = true;
		}

		return code;
	}

	bool FileExists(void) { return m_bFileExists; }
	bool IsModified(void) { return m_bModified;   }

	virtual void UpdateViews(void) {}

	bool m_bFileExists;
	bool m_bModified;

protected:
	TDoc* m_pDoc;
	TView* m_pView;
	std::string m_sFileName;
};

#endif
