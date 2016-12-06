#ifndef	__PDVER_H
#define	__PDVER_H

// VERSIONINFO リソースを読む
class ProjectRCVersion {
public:
    ProjectRCVersion ( const tchar *appFName );	// appFNameは TModule::GetModuleFileName()で得られる
    virtual ~ProjectRCVersion ();
	virtual bool GetProductVersion(tnstr &prodVersion);

protected:
	void *FVData;

private:
	// このオブジェクトはコピーしてはならない
	ProjectRCVersion (const ProjectRCVersion &);
	ProjectRCVersion & operator =(const ProjectRCVersion &);
};

// Complicated version. :-)
class ProjectRCVersionString : public ProjectRCVersion {
typedef ProjectRCVersion super;
protected:
	LPBYTE      TransBlock;
public:
	ProjectRCVersionString(const tchar *appFName);
	bool GetProductName(tnstr &prodName);
//	bool GetProductVersion(tnstr &prodVersion);
	bool GetCopyright (tnstr &copyright);
	bool GetDebug (tnstr &debug);
};

int CompareVersion(const tchar *v1, const tchar *v2);

#endif	__PDVER_H

