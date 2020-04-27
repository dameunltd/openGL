#pragma once

class VertexBuffer
{
private:
	/* ID is created as integer for every object created
	   ~ internal renderer ID */
	unsigned int m_RendererID;
public:
	VertexBuffer(const void* data, unsigned int size); /* constructor */
	~VertexBuffer(); /* destructor */

	/* these two function bind and unbinds vertex buffer*/
	void Bind() const;
	void Unbind() const;
};