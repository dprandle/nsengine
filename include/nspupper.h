/*
This is the pup framework - inspired by Dr. Lawlor in
computer science department at University of Alaska Fairbanks
who is responsible for the entire pup design concept
*/

#ifndef NSPUPPER_H
#define NSPUPPER_H

#define PUP_IN 0
#define PUP_OUT 1

#include <nsglobal.h>

class NSFilePUPer
{
public:
	enum PupType {
		Binary,
		Text
	};

	NSFilePUPer(PupType t_, nsfstream & fileStream, const nsuint & _io);
	const nsuint & mode() const;
	PupType type();

	nsfstream & fs;

protected:
	nsuint io;
	PupType t;
};

class NSBinFilePUPer : public NSFilePUPer
{
public:

	NSBinFilePUPer(nsfstream & fileStream, const nsuint & _io);
	friend void pup(NSBinFilePUPer & p, nschar & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, nswchar & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, nssint & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, nsint & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, nslint & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, nsllint & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, nsuchar & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, nsusint & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, nsuint & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, nsulint & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, nsullint & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, nsfloat & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, nsdouble & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, nsldouble & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, nsbool & val, const nsstring & varName);

	template <class T>
	friend void pupbytes(NSBinFilePUPer & p, T & val);
};

class NSTextFilePUPer : public NSFilePUPer
{
public:

	NSTextFilePUPer(nsfstream & fileStream, const nsuint & _io);
	friend void pup(NSTextFilePUPer & p, nschar & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, nswchar & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, nssint & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, nsint & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, nslint & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, nsllint & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, nsuchar & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, nsusint & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, nsuint & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, nsulint & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, nsullint & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, nsfloat & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, nsdouble & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, nsldouble & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, nsbool & val, const nsstring & varName);
	
	template<class PUPer>
	friend void pup(PUPer & p, nsstring & str, const nsstring & varName);
};

template <class T>
void pupbytes(NSBinFilePUPer & p, T & val)
{
	if (p.io == PUP_IN)
		p.fs.read((char*)&val, sizeof(T));
	else
		p.fs.write((char*)&val, sizeof(T));
}

// STL Types
template<class PUPer, class T>
void pup(PUPer & p, std::vector<T> & vec, const nsstring & varName)
{
	nsuint size = vec.size();
	pup(p, size, varName + ".size");
	vec.resize(size);
	for (nsuint i = 0; i < size; ++i)
		pup(p, vec[i], varName + "[" + std::to_string(i) + "]");
}

template<class PUPer, class T1, class T2>
void pup(PUPer & p, std::pair<T1,T2> & pr, const nsstring & varName)
{
	pup(p, pr.first, varName + ".first");
	pup(p, pr.second, varName + ".second");
}

template<class PUPer, class Key, class Val>
void pup(PUPer & p, std::map<Key, Val> & m, const nsstring & varName)
{
	nsuint size = m.size();
	pup(p, size, varName + ".size");
	if (p.mode() == PUP_IN)
	{
		for (nsuint i = 0; i < size; ++i)
		{
			Key k; Val v;
			pup(p, k, varName + ".key");
			pup(p, v, varName + "[Key]");
			m.emplace(k, v);
		}
	}
	else
	{
		auto iter = m.begin();
		while (iter != m.end())
		{
			Key k = iter->first; Val v = iter->second;
			pup(p, k, varName + ".key");
			pup(p, v, varName + "[Key]");
			++iter;
		}
	}
}

template<class PUPer, class Key, class Val>
void pup(PUPer & p, std::unordered_map<Key, Val> & m, const nsstring & varName = "")
{
	nsuint size = m.size();
	pup(p, size, varName + ".size");
	if (p.mode() == PUP_IN)
	{
		for (nsuint i = 0; i < size; ++i)
		{
			Key k = Key(); Val v = Val();
			pup(p, k, varName + ".key");
			pup(p, v, varName + "[Key]");
			m.emplace(k, v);
		}
	}
	else
	{
		auto iter = m.begin();
		while (iter != m.end())
		{
			Key k = iter->first; Val v = iter->second;
			pup(p, k, varName + ".key");
			pup(p, v, varName + "[Key]");
			++iter;
		}
	}
}

template<class PUPer, class Key, class Val>
void pup(PUPer & p, std::multimap<Key, Val> & m, const nsstring & varName = "")
{
	nsuint size = m.size();
	pup(p, size, varName + ".size");
	if (p.mode() == PUP_IN)
	{
		for (nsuint i = 0; i < size; ++i)
		{
			Key k; Val v;
			pup(p, k, varName + ".key");
			pup(p, v, varName + "[Key]");
			m.emplace(k, v);
		}
	}
	else
	{
		auto iter = m.begin();
		while (iter != m.end())
		{
			Key k = iter->first; Val v = iter->second;
			pup(p, k, varName + ".key");
			pup(p, v, varName + "[Key]");
			++iter;
		}
	}
}

template<class PUPer, class Key, class Val>
void pup(PUPer & p, std::unordered_multimap<Key, Val> & m, const nsstring & varName = "")
{
	nsuint size = m.size();
	pup(p, size, varName + ".size");
	if (p.mode() == PUP_IN)
	{
		for (nsuint i = 0; i < size; ++i)
		{
			Key k; Val v;
			pup(p, k, varName + ".key");
			pup(p, v, varName + "[Key]");
			m.emplace(k, v);
		}
	}
	else
	{
		auto iter = m.begin();
		while (iter != m.end())
		{
			Key k = iter->first; Val v = iter->second;
			pup(p, k, varName + ".key");
			pup(p, v, varName + "[Key]");
			++iter;
		}
	}
}

template<class PUPer>
struct Helper;

template<class PUPer>
void pup(PUPer & p, nsstring & str, const nsstring & varName) { Helper<PUPer>::pup(p, str, varName); }

template<class PUPer>
struct Helper
{
	static void pup(PUPer & p, nsstring & str, const nsstring & varName)
	{
		nsuint size = str.size();
		::pup(p, size, varName);
		str.resize(size);
		for (nsuint i = 0; i < size; ++i)
			::pup(p, str[i], varName);
	}
};

template<>
struct Helper<NSTextFilePUPer>
{
	static void pup(NSTextFilePUPer & p, nsstring & str, const nsstring & varName)
	{
		nsstring begtag, endtag;
		begtag = varName + ":<nsstring>"; endtag = "</nsstring>";

		if (p.mode() == PUP_OUT)
		{
			p.fs << begtag << str << endtag << "\n";
		}
		else
		{
			nsstring line;
			nsuint loc = 0;
			std::getline(p.fs, line);
			nsuint beg = begtag.size(); loc = line.find(endtag);
			if (loc != nsstring::npos)
			{
				line = line.substr(beg, loc - beg);
				str += (line);
			}
			else
			{
				line = line.substr(beg);
				str += (line + "\n");
			}

			while (loc == nsstring::npos)
			{
				std::getline(p.fs, line);
				loc = line.find(endtag);
				if (loc != nsstring::npos)
					line = line.substr(0, loc);
				str += (line + "\n");
			}

		}
	}
};



template<class PUPer, class T>
void pup(PUPer & p, std::set<T> & s, const nsstring & varName)
{
	nsuint size = s.size();
	pup(p, size, varName + ".size");
	if (p.mode() == PUP_IN)
	{
		for (nsuint i = 0; i < size; ++i)
		{
			T val;
			pup(p, val, varName);
			s.emplace(val);
		}
	}
	else
	{
		auto iter = s.begin();
		while (iter != s.end())
		{
			T val = *iter;
			pup(p, val, varName);
			++iter;
		}
	}
}

template<class PUPer, class T>
void pup(PUPer & p, std::unordered_set<T> & s, const nsstring & varName)
{
	nsuint size = s.size();
	pup(p, size, varName + ".size");
	if (p.mode() == PUP_IN)
	{
		for (nsuint i = 0; i < size; ++i)
		{
			T val;
			pup(p, val, varName);
			s.emplace(val);
		}
	}
	else
	{
		auto iter = s.begin();
		while (iter != s.end())
		{
			T val = *iter;
			pup(p, val, varName);
			++iter;
		}
	}
}

template<class PUPer, class T>
void pup(PUPer & p, std::multiset<T> & s, const nsstring & varName)
{
	nsuint size = s.size();
	pup(p, size, varName + ".size");
	if (p.mode() == PUP_IN)
	{
		for (nsuint i = 0; i < size; ++i)
		{
			T val;
			pup(p, val, varName);
			s.emplace(val);
		}
	}
	else
	{
		auto iter = s.begin();
		while (iter != s.end())
		{
			T val = *iter;
			pup(p, val, varName);
			++iter;
		}
	}
}

#endif