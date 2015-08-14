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

	NSFilePUPer(PupType t_, nsfstream & fileStream, const uint32 & _io);
	const uint32 & mode() const;
	PupType type();

	nsfstream & fs;

protected:
	uint32 io;
	PupType t;
};

class NSBinFilePUPer : public NSFilePUPer
{
public:

	NSBinFilePUPer(nsfstream & fileStream, const uint32 & _io);
	friend void pup(NSBinFilePUPer & p, char & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, wchar & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, char16 & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, char32 & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, int8 & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, int16 & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, int32 & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, int64 & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, uint8 & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, uint16 & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, uint32 & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, uint64 & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, float & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, double & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, ldouble & val, const nsstring & varName);
	friend void pup(NSBinFilePUPer & p, bool & val, const nsstring & varName);

	template <class T>
	friend void pupbytes(NSBinFilePUPer & p, T & val);
};

class NSTextFilePUPer : public NSFilePUPer
{
public:

	NSTextFilePUPer(nsfstream & fileStream, const uint32 & _io);
	friend void pup(NSTextFilePUPer & p, char & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, wchar & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, char16 & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, char32 & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, int8 & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, int16 & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, int32 & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, int64 & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, uint8 & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, uint16 & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, uint32 & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, uint64 & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, float & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, double & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, ldouble & val, const nsstring & varName);
	friend void pup(NSTextFilePUPer & p, bool & val, const nsstring & varName);
	
	template<class PUPer>
	friend void pup(PUPer & p, nsstring & str, const nsstring & varName);
};

void pup(NSBinFilePUPer & p, char & val, const nsstring & varName);
void pup(NSBinFilePUPer & p, wchar & val, const nsstring & varName);
void pup(NSBinFilePUPer & p, char16 & val, const nsstring & varName);
void pup(NSBinFilePUPer & p, char32 & val, const nsstring & varName);
void pup(NSBinFilePUPer & p, int8 & val, const nsstring & varName);
void pup(NSBinFilePUPer & p, int16 & val, const nsstring & varName);
void pup(NSBinFilePUPer & p, int32 & val, const nsstring & varName);
void pup(NSBinFilePUPer & p, int64 & val, const nsstring & varName);
void pup(NSBinFilePUPer & p, uint8 & val, const nsstring & varName);
void pup(NSBinFilePUPer & p, uint16 & val, const nsstring & varName);
void pup(NSBinFilePUPer & p, uint32 & val, const nsstring & varName);
void pup(NSBinFilePUPer & p, uint64 & val, const nsstring & varName);
void pup(NSBinFilePUPer & p, float & val, const nsstring & varName);
void pup(NSBinFilePUPer & p, double & val, const nsstring & varName);
void pup(NSBinFilePUPer & p, ldouble & val, const nsstring & varName);
void pup(NSBinFilePUPer & p, bool & val, const nsstring & varName);

void pup(NSTextFilePUPer & p, char & val, const nsstring & varName);
void pup(NSTextFilePUPer & p, wchar & val, const nsstring & varName);
void pup(NSTextFilePUPer & p, char16 & val, const nsstring & varName);
void pup(NSTextFilePUPer & p, char32 & val, const nsstring & varName);
void pup(NSTextFilePUPer & p, int8 & val, const nsstring & varName);
void pup(NSTextFilePUPer & p, int16 & val, const nsstring & varName);
void pup(NSTextFilePUPer & p, int32 & val, const nsstring & varName);
void pup(NSTextFilePUPer & p, int64 & val, const nsstring & varName);
void pup(NSTextFilePUPer & p, uint8 & val, const nsstring & varName);
void pup(NSTextFilePUPer & p, uint16 & val, const nsstring & varName);
void pup(NSTextFilePUPer & p, uint32 & val, const nsstring & varName);
void pup(NSTextFilePUPer & p, uint64 & val, const nsstring & varName);
void pup(NSTextFilePUPer & p, float & val, const nsstring & varName);
void pup(NSTextFilePUPer & p, double & val, const nsstring & varName);
void pup(NSTextFilePUPer & p, ldouble & val, const nsstring & varName);
void pup(NSTextFilePUPer & p, bool & val, const nsstring & varName);

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
	uint32 size = static_cast<uint32>(vec.size());
	pup(p, size, varName + ".size");
	vec.resize(size);
	for (uint32 i = 0; i < size; ++i)
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
	uint32 size = static_cast<uint32>(m.size());
	pup(p, size, varName + ".size");
	if (p.mode() == PUP_IN)
	{
		for (uint32 i = 0; i < size; ++i)
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

template<class PUPer, class Key, class Val, class EH>
void pup(PUPer & p, std::unordered_map<Key, Val, EH> & m, const nsstring & varName = "")
{
	uint32 size = static_cast<uint32>(m.size());
	pup(p, size, varName + ".size");
	if (p.mode() == PUP_IN)
	{
		for (uint32 i = 0; i < size; ++i)
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
	uint32 size = m.size();
	pup(p, size, varName + ".size");
	if (p.mode() == PUP_IN)
	{
		for (uint32 i = 0; i < size; ++i)
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

template<class PUPer, class Key, class Val, class EH>
void pup(PUPer & p, std::unordered_multimap<Key, Val, EH> & m, const nsstring & varName = "")
{
	uint32 size = static_cast<uint32>(m.size());
	pup(p, size, varName + ".size");
	if (p.mode() == PUP_IN)
	{
		for (uint32 i = 0; i < size; ++i)
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
		uint32 size = static_cast<uint32>(str.size());
		::pup(p, size, varName);
		str.resize(size);
		for (uint32 i = 0; i < size; ++i)
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
			size_t loc = 0;
			std::getline(p.fs, line);
		    size_t beg = begtag.size(); loc = line.find(endtag);
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
	uint32 size = static_cast<uint32>(s.size());
	pup(p, size, varName + ".size");
	if (p.mode() == PUP_IN)
	{
		for (uint32 i = 0; i < size; ++i)
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

template<class PUPer, class T, class EH>
void pup(PUPer & p, std::unordered_set<T, EH> & s, const nsstring & varName)
{
	uint32 size = static_cast<uint32>(s.size());
	pup(p, size, varName + ".size");
	if (p.mode() == PUP_IN)
	{
		for (uint32 i = 0; i < size; ++i)
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
	uint32 size = s.size();
	pup(p, size, varName + ".size");
	if (p.mode() == PUP_IN)
	{
		for (uint32 i = 0; i < size; ++i)
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
