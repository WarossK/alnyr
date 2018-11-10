#pragma once

#include <functional>

namespace alnyr
{
	//deprecated
	template<class Type>
	class alnyrProperty
	{
	public:
		using Setter = std::function<Type(Type&, const Type&)>;
		using Getter = std::function<Type(Type&)>;

	private:
		Type value_;
		Setter setter_;
		Getter getter_;

	public:
		alnyrProperty()
		{
			setter_ = [](Type& v, const Type& i) -> Type
			{ v = i; return v; };
			getter_ = [](Type& v) -> Type
			{ return v; };
		}

		alnyrProperty(Setter setter)
		{
			setter_ = setter;
			getter_ = [](Type& v) -> Type
			{ return v; };
		}

		alnyrProperty(Getter getter)
		{
			getter_ = getter;
			setter_ = [](Type& v, const Type& i) -> Type
			{ v = i; return v; };
		}

		Type& operator = (const Type& rhs)
		{
			return setter_(value_, rhs);
		}

		operator Type()
		{
			return getter_(value_);
		}

		Type* operator -> ()
		{
			return &value_;
		}

		size_t size()
		{
			return sizeof(Type);
		}
	};
}