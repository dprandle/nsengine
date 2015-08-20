/*
This is the pup framework - inspired by Dr. Lawlor in
computer science department at University of Alaska Fairbanks
who is responsible for the entire pup design concept
*/

#ifndef NSPUPPER_H
#define NSPUPPER_H

#define PUP_IN 0
#define PUP_OUT 1

#include <nsfstream.h>
#include <nsset.h>
#include <nsunordered_set.h>
#include <nsmap.h>
#include <nsunordered_map.h>

class nsfile_pupper
{
public:
	enum pup_t {
		pup_binary,
		pup_text
	};

	nsfile_pupper(pup_t t_, nsfstream & file_stream_, const uint32 & _io);
	const uint32 & mode() const;
	pup_t type();

	nsfstream & fs;

protected:
	uint32 m_io;
	pup_t m_type;
};

class nsbinary_file_pupper : public nsfile_pupper
{
public:

	nsbinary_file_pupper(nsfstream & file_stream_, const uint32 & _io);
	friend void pup(nsbinary_file_pupper & p_, char & val_, const nsstring & var_name_);
	friend void pup(nsbinary_file_pupper & p_, wchar & val_, const nsstring & var_name_);
	friend void pup(nsbinary_file_pupper & p_, char16 & val_, const nsstring & var_name_);
	friend void pup(nsbinary_file_pupper & p_, char32 & val_, const nsstring & var_name_);
	friend void pup(nsbinary_file_pupper & p_, int8 & val_, const nsstring & var_name_);
	friend void pup(nsbinary_file_pupper & p_, int16 & val_, const nsstring & var_name_);
	friend void pup(nsbinary_file_pupper & p_, int32 & val_, const nsstring & var_name_);
	friend void pup(nsbinary_file_pupper & p_, int64 & val_, const nsstring & var_name_);
	friend void pup(nsbinary_file_pupper & p_, uint8 & val_, const nsstring & var_name_);
	friend void pup(nsbinary_file_pupper & p_, uint16 & val_, const nsstring & var_name_);
	friend void pup(nsbinary_file_pupper & p_, uint32 & val_, const nsstring & var_name_);
	friend void pup(nsbinary_file_pupper & p_, uint64 & val_, const nsstring & var_name_);
	friend void pup(nsbinary_file_pupper & p_, float & val_, const nsstring & var_name_);
	friend void pup(nsbinary_file_pupper & p_, double & val_, const nsstring & var_name_);
	friend void pup(nsbinary_file_pupper & p_, ldouble & val_, const nsstring & var_name_);
	friend void pup(nsbinary_file_pupper & p_, bool & val_, const nsstring & var_name_);

	template <class T>
	friend void pup_bytes(nsbinary_file_pupper & p_, T & val_);
};

class nstext_file_pupper : public nsfile_pupper
{
public:

	nstext_file_pupper(nsfstream & file_stream_, const uint32 & _io);
	friend void pup(nstext_file_pupper & p_, char & val_, const nsstring & var_name_);
	friend void pup(nstext_file_pupper & p_, wchar & val_, const nsstring & var_name_);
	friend void pup(nstext_file_pupper & p_, char16 & val_, const nsstring & var_name_);
	friend void pup(nstext_file_pupper & p_, char32 & val_, const nsstring & var_name_);
	friend void pup(nstext_file_pupper & p_, int8 & val_, const nsstring & var_name_);
	friend void pup(nstext_file_pupper & p_, int16 & val_, const nsstring & var_name_);
	friend void pup(nstext_file_pupper & p_, int32 & val_, const nsstring & var_name_);
	friend void pup(nstext_file_pupper & p_, int64 & val_, const nsstring & var_name_);
	friend void pup(nstext_file_pupper & p_, uint8 & val_, const nsstring & var_name_);
	friend void pup(nstext_file_pupper & p_, uint16 & val_, const nsstring & var_name_);
	friend void pup(nstext_file_pupper & p_, uint32 & val_, const nsstring & var_name_);
	friend void pup(nstext_file_pupper & p_, uint64 & val_, const nsstring & var_name_);
	friend void pup(nstext_file_pupper & p_, float & val_, const nsstring & var_name_);
	friend void pup(nstext_file_pupper & p_, double & val_, const nsstring & var_name_);
	friend void pup(nstext_file_pupper & p_, ldouble & val_, const nsstring & var_name_);
	friend void pup(nstext_file_pupper & p_, bool & val_, const nsstring & var_name_);
	
	template<class PUPer>
	friend void pup(PUPer & p_, nsstring & str, const nsstring & var_name_);
};

void pup(nsbinary_file_pupper & p_, char & val_, const nsstring & var_name_);
void pup(nsbinary_file_pupper & p_, wchar & val_, const nsstring & var_name_);
void pup(nsbinary_file_pupper & p_, char16 & val_, const nsstring & var_name_);
void pup(nsbinary_file_pupper & p_, char32 & val_, const nsstring & var_name_);
void pup(nsbinary_file_pupper & p_, int8 & val_, const nsstring & var_name_);
void pup(nsbinary_file_pupper & p_, int16 & val_, const nsstring & var_name_);
void pup(nsbinary_file_pupper & p_, int32 & val_, const nsstring & var_name_);
void pup(nsbinary_file_pupper & p_, int64 & val_, const nsstring & var_name_);
void pup(nsbinary_file_pupper & p_, uint8 & val_, const nsstring & var_name_);
void pup(nsbinary_file_pupper & p_, uint16 & val_, const nsstring & var_name_);
void pup(nsbinary_file_pupper & p_, uint32 & val_, const nsstring & var_name_);
void pup(nsbinary_file_pupper & p_, uint64 & val_, const nsstring & var_name_);
void pup(nsbinary_file_pupper & p_, float & val_, const nsstring & var_name_);
void pup(nsbinary_file_pupper & p_, double & val_, const nsstring & var_name_);
void pup(nsbinary_file_pupper & p_, ldouble & val_, const nsstring & var_name_);
void pup(nsbinary_file_pupper & p_, bool & val_, const nsstring & var_name_);

void pup(nstext_file_pupper & p_, char & val_, const nsstring & var_name_);
void pup(nstext_file_pupper & p_, wchar & val_, const nsstring & var_name_);
void pup(nstext_file_pupper & p_, char16 & val_, const nsstring & var_name_);
void pup(nstext_file_pupper & p_, char32 & val_, const nsstring & var_name_);
void pup(nstext_file_pupper & p_, int8 & val_, const nsstring & var_name_);
void pup(nstext_file_pupper & p_, int16 & val_, const nsstring & var_name_);
void pup(nstext_file_pupper & p_, int32 & val_, const nsstring & var_name_);
void pup(nstext_file_pupper & p_, int64 & val_, const nsstring & var_name_);
void pup(nstext_file_pupper & p_, uint8 & val_, const nsstring & var_name_);
void pup(nstext_file_pupper & p_, uint16 & val_, const nsstring & var_name_);
void pup(nstext_file_pupper & p_, uint32 & val_, const nsstring & var_name_);
void pup(nstext_file_pupper & p_, uint64 & val_, const nsstring & var_name_);
void pup(nstext_file_pupper & p_, float & val_, const nsstring & var_name_);
void pup(nstext_file_pupper & p_, double & val_, const nsstring & var_name_);
void pup(nstext_file_pupper & p_, ldouble & val_, const nsstring & var_name_);
void pup(nstext_file_pupper & p_, bool & val_, const nsstring & var_name_);

template <class T>
void pup_bytes(nsbinary_file_pupper & p_, T & val_)
{
	if (p_.m_io == PUP_IN)
		p_.fs.read((char*)&val_, sizeof(T));
	else
		p_.fs.write((char*)&val_, sizeof(T));
}

// STL Types
template<class PUPer, class T>
void pup(PUPer & p_, std::vector<T> & vec, const nsstring & var_name_)
{
	uint32 size = static_cast<uint32>(vec.size());
	pup(p_, size, var_name_ + ".size");
	vec.resize(size);
	for (uint32 i = 0; i < size; ++i)
		pup(p_, vec[i], var_name_ + "[" + std::to_string(i) + "]");
}

template<class PUPer, class T1, class T2>
void pup(PUPer & p_, std::pair<T1,T2> & pr, const nsstring & var_name_)
{
	pup(p_, pr.first, var_name_ + ".first");
	pup(p_, pr.second, var_name_ + ".second");
}

template<class PUPer, class Key, class Val>
void pup(PUPer & p_, std::map<Key, Val> & m, const nsstring & var_name_)
{
	uint32 size = static_cast<uint32>(m.size());
	pup(p_, size, var_name_ + ".size");
	if (p_.mode() == PUP_IN)
	{
		for (uint32 i = 0; i < size; ++i)
		{
			Key k; Val v;
			pup(p_, k, var_name_ + ".key");
			pup(p_, v, var_name_ + "[Key]");
			m.emplace(k, v);
		}
	}
	else
	{
		auto iter = m.begin();
		while (iter != m.end())
		{
			Key k = iter->first; Val v = iter->second;
			pup(p_, k, var_name_ + ".key");
			pup(p_, v, var_name_ + "[Key]");
			++iter;
		}
	}
}

template<class PUPer, class Key, class Val, class EH>
void pup(PUPer & p_, std::unordered_map<Key, Val, EH> & m, const nsstring & var_name_ = "")
{
	uint32 size = static_cast<uint32>(m.size());
	pup(p_, size, var_name_ + ".size");
	if (p_.mode() == PUP_IN)
	{
		for (uint32 i = 0; i < size; ++i)
		{
			Key k = Key(); Val v = Val();
			pup(p_, k, var_name_ + ".key");
			pup(p_, v, var_name_ + "[Key]");
			m.emplace(k, v);
		}
	}
	else
	{
		auto iter = m.begin();
		while (iter != m.end())
		{
			Key k = iter->first; Val v = iter->second;
			pup(p_, k, var_name_ + ".key");
			pup(p_, v, var_name_ + "[Key]");
			++iter;
		}
	}
}

template<class PUPer, class Key, class Val>
void pup(PUPer & p_, std::multimap<Key, Val> & m, const nsstring & var_name_ = "")
{
	uint32 size = m.size();
	pup(p_, size, var_name_ + ".size");
	if (p_.mode() == PUP_IN)
	{
		for (uint32 i = 0; i < size; ++i)
		{
			Key k; Val v;
			pup(p_, k, var_name_ + ".key");
			pup(p_, v, var_name_ + "[Key]");
			m.emplace(k, v);
		}
	}
	else
	{
		auto iter = m.begin();
		while (iter != m.end())
		{
			Key k = iter->first; Val v = iter->second;
			pup(p_, k, var_name_ + ".key");
			pup(p_, v, var_name_ + "[Key]");
			++iter;
		}
	}
}

template<class PUPer, class Key, class Val, class EH>
void pup(PUPer & p_, std::unordered_multimap<Key, Val, EH> & m, const nsstring & var_name_ = "")
{
	uint32 size = static_cast<uint32>(m.size());
	pup(p_, size, var_name_ + ".size");
	if (p_.mode() == PUP_IN)
	{
		for (uint32 i = 0; i < size; ++i)
		{
			Key k; Val v;
			pup(p_, k, var_name_ + ".key");
			pup(p_, v, var_name_ + "[Key]");
			m.emplace(k, v);
		}
	}
	else
	{
		auto iter = m.begin();
		while (iter != m.end())
		{
			Key k = iter->first; Val v = iter->second;
			pup(p_, k, var_name_ + ".key");
			pup(p_, v, var_name_ + "[Key]");
			++iter;
		}
	}
}

template<class PUPer>
struct Helper;

template<class PUPer>
void pup(PUPer & p_, nsstring & str, const nsstring & var_name_) { Helper<PUPer>::pup(p_, str, var_name_); }

template<class PUPer>
struct Helper
{
	static void pup(PUPer & p_, nsstring & str, const nsstring & var_name_)
	{
		uint32 size = static_cast<uint32>(str.size());
		::pup(p_, size, var_name_);
		str.resize(size);
		for (uint32 i = 0; i < size; ++i)
			::pup(p_, str[i], var_name_);
	}
};

template<>
struct Helper<nstext_file_pupper>
{
	static void pup(nstext_file_pupper & p_, nsstring & str, const nsstring & var_name_)
	{
		nsstring begtag, endtag;
		begtag = var_name_ + ":<nsstring>"; endtag = "</nsstring>";

		if (p_.mode() == PUP_OUT)
		{
			p_.fs << begtag << str << endtag << "\n";
		}
		else
		{
			nsstring line;
			size_t loc = 0;
			std::getline(p_.fs, line);
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
				std::getline(p_.fs, line);
				loc = line.find(endtag);
				if (loc != nsstring::npos)
					line = line.substr(0, loc);
				str += (line + "\n");
			}

		}
	}
};



template<class PUPer, class T>
void pup(PUPer & p_, std::set<T> & s, const nsstring & var_name_)
{
	uint32 size = static_cast<uint32>(s.size());
	pup(p_, size, var_name_ + ".size");
	if (p_.mode() == PUP_IN)
	{
		for (uint32 i = 0; i < size; ++i)
		{
			T val_;
			pup(p_, val_, var_name_);
			s.emplace(val_);
		}
	}
	else
	{
		auto iter = s.begin();
		while (iter != s.end())
		{
			T val_ = *iter;
			pup(p_, val_, var_name_);
			++iter;
		}
	}
}

template<class PUPer, class T, class EH>
void pup(PUPer & p_, std::unordered_set<T, EH> & s, const nsstring & var_name_)
{
	uint32 size = static_cast<uint32>(s.size());
	pup(p_, size, var_name_ + ".size");
	if (p_.mode() == PUP_IN)
	{
		for (uint32 i = 0; i < size; ++i)
		{
			T val_;
			pup(p_, val_, var_name_);
			s.emplace(val_);
		}
	}
	else
	{
		auto iter = s.begin();
		while (iter != s.end())
		{
			T val_ = *iter;
			pup(p_, val_, var_name_);
			++iter;
		}
	}
}

template<class PUPer, class T>
void pup(PUPer & p_, std::multiset<T> & s, const nsstring & var_name_)
{
	uint32 size = s.size();
	pup(p_, size, var_name_ + ".size");
	if (p_.mode() == PUP_IN)
	{
		for (uint32 i = 0; i < size; ++i)
		{
			T val_;
			pup(p_, val_, var_name_);
			s.emplace(val_);
		}
	}
	else
	{
		auto iter = s.begin();
		while (iter != s.end())
		{
			T val_ = *iter;
			pup(p_, val_, var_name_);
			++iter;
		}
	}
}

#endif
