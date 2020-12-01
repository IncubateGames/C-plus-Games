#ifndef _Dynamic_Object_
#define _Dynamic_Object_

class DynamicObject
{
private:
	int index;
	int ImageIndex;
	bool _enable;
	const FloatRect *rect;
public:
	DynamicObject(const int& imageindex,const FloatRect& rect):ImageIndex(imageindex),rect(&rect)
	{
		_enable = true;
	};

	const int& GetIndex() const
	{
		return index;
	};

	const int& GetImageIndex() const
	{
		return ImageIndex;
	};

	const FloatRect& GetRect() const
	{
		return *rect;
	};

	void Inicializar(const int& index)
	{
		this->index = index;		
	};

	void Disable()
	{
		_enable = false;
	};

	const bool& Enable()  const
	{
		return _enable;
	};
};


#endif