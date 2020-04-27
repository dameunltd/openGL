#pragma once

class IndexBuffer
{
private:
	/* ID is created as integer for every object created
	   ~ internal renderer ID */
	unsigned int m_RendererID;
	unsigned int m_Count;
public:
	IndexBuffer(const unsigned int* data, unsigned int count); /* constructor */
	~IndexBuffer(); /* destructor */

	/* these two function bind and unbinds vertex buffer*/
	void Bind() const;
	void Unbind() const;
	
	/* Getter to store and return count */
	inline unsigned int GetCount() const { return m_Count; }
};