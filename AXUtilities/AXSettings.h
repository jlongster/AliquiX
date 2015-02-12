/* --------------------------------------------------------------
	File:			AXSettings.h
	Description:	This file defines a class that manages the application's settings.  It can
					parse settings from a file as well as save the current settings to a file.

					It keeps track of different types of settings so that you can group settings
					together.  It comes with two types, User and System, which define a kind of
					higher/lower level relationship.  However, you can easily define your own types.
					If you wanted to group setting for the terrain together, you could easily create
					a SETTING_TERRAIN type as long as its integer value is unique.

					When writing to a file, you must specify the file name as well as the type of
					setting that you want to write.  This way you can write the user settings in the
					general file but write the terrain settings in its own secret file.

					It uses boost::any to store a generic setting and templates to call functions on
					each setting.  For now it only supports the following types, but it can easily be extended:
						- Int
						- Float
						- String

					This object has not been profiled yet, and since access involves a lookup into a Map,
					frequent accesses may cause performance to slow.  It is recommended to cache settings
					if possible.

					This object is not intended to be used during application run time anyway.  You should
					extract the settings at startup and save them at shutdown; do not abuse it.
	Date:			July 1, 2005
	Author:			James Long

-------------------------------------------------------------- */

#if !defined(AXSETTINGS_INCLUDED)
#define AXSETTINGS_INCLUDED

#include "..\\AXCommon.h"

#include <string>
#include <map>

#include "boost\\any.hpp"


class AXSettings {
public:
	AXSettings();
	virtual ~AXSettings();

	template<class T>
	void SetSetting(std::string name, T value) {
		_SettingsMap[name] = value;
	}


	template<class T>
	T GetSetting(std::string name) {
		StringAnyMap::iterator it;

		try {
			if((it = _SettingsMap.find(name)) != _SettingsMap.end()) {
				return boost::any_cast<T>((*it).second);
			}
			else {
				return static_cast<T>(ERRORINT);
			}
		}
		catch (boost::bad_any_cast &) {
			return static_cast<T>(ERRORINT);
		}
	}

	/*	To make it fully generic (well, for our 3 supported types), we need to declare a slightly different function when working with std::string */
	template<>
	std::string GetSetting<std::string>(std::string name) {
		StringAnyMap::iterator it;

		try {
			if((it = _SettingsMap.find(name)) != _SettingsMap.end())
				return boost::any_cast<std::string>((*it).second);
			else {
				return std::string("defaultstring");
			}
		}
		catch (boost::bad_any_cast &) {
			return std::string("defaultstring");
		}
	}

	AXResult ParseSettings(std::string SectionName);
	AXResult WriteSettings();

	static void SetFileName(std::string Name);
	static const char* GetFileName();

	void DebugSettings();

private:
	typedef std::map<std::string, boost::any> StringAnyMap;

	static std::string FileName;
	std::string _SectionName;

	StringAnyMap _SettingsMap;
};

#endif
