#pragma once

#include <rgde/render/vertices.h>

namespace render
{
	// Primitives supported by draw-primitive API
	enum PrimitiveType{
		PrimTypePointList             = 1,
		PrimTypeLineList              = 2,
		PrimTypeLineStrip             = 3,
		PrimTypeTriangleList          = 4,
		PrimTypeTriangleStrip         = 5,
		PrimTypeTriangleFan           = 6,
		PrimTypeFORCE_DWORD           = 0x7fffffff, /* force 32-bit size enum */
	};

	class IGeometry
	{
	public:
		virtual ~IGeometry(){}
		static IGeometry* create(const vertex::VertexDecl decl, bool isDynamic);
		virtual void update(const void* pdata, size_t bytes, size_t size_of_vertex) = 0;
		virtual void render(PrimitiveType ePrimType, unsigned int nPrimNum) = 0;		
	};

    template<class Vertex>
	class TGeometry
	{
	public:
		typedef std::vector<Vertex> Vertexes;

		TGeometry(bool isDynamic = false) : 
			m_spImpl(IGeometry::create(Vertex::GetDecl(), isDynamic)),
			m_bIsDynamic(isDynamic)
		{			
			m_bChanged = false;
		}

		bool isDynamic() const {return m_bIsDynamic;}

		void render(PrimitiveType ePrimType = PrimTypeTriangleList)
		{
			unsigned nNumVertices = static_cast<unsigned>(m_vVertexes.size());
			unsigned nPrimNum = nNumVertices;
			switch( ePrimType )
			{
			case PrimTypeTriangleList:
				nPrimNum /= 3;
				break;

			case PrimTypeLineList:
				nPrimNum /= 2;
				break;

			default:
				break;
				//assert(false); //VS8 issue
			}

			m_spImpl->render(ePrimType, nPrimNum);
		}

		void load( const std::string& filename )
		{
			//std::vector<byte> data;
			//io::CFileSystem& fs = io::TheFileSystem::get();

			////first try to find binary file
			//std::wstring bin_filename = filename + L".mesh";
			//io::readstream_ptr bin_in = fs.findFile(bin_filename);
			////io::IReadStream& rs = *bin_in

			//uint vertex_size = 0;//Vertex size
			//uint vdata_size = 0;//Vertex Data Size
			//(*bin_in) >> vertex_size >> vdata_size;
			////Vertex Data


			//io::readstream_ptr in = fs.findFile(filename);
			//unsigned int size = in->getSize();
			//io::stream_to_vector(data, in);

			//TiXmlDocument xml;//( xml_filename );
			//xml.Parse((const char*)&(data[0]));
			////TiXmlDocument xml( xml_filename ) ;

			////if ( !xml.LoadFile() )
			////	return 0;

			//load( xml.FirstChild( "mesh" ) );
		}

		void load( TiXmlNode* root_geom_node )
		{
			//TiXmlElement *elem = root_geom_node->FirstChildElement("faces");            
			//TiXmlElement* ev = 0;

			//elem = root_geom_node->FirstChildElement("vertices");

			//ev = 0;
			//ev = elem->IterateChildren("vertex", ev)->ToElement();

			//int vertex_count;
			//elem->Attribute( "num", &vertex_count ); 

			//Vertexes::iterator vi,end;
			//m_vVertexes.resize( vertex_count );

			//vi = m_vVertexes.begin();
			//end = m_vVertexes.end();
			//for ( vi; vi != end; ++vi)
			//{
			//	XmlVertexReader< Vertex >::Read( ev, (*vi) );
			//	ev = elem->IterateChildren("vertex", ev)->ToElement()
			//};
		}

		Vertexes& lock() {return m_vVertexes;}

		void unlock()
		{
			const size_t nVertexSize = sizeof(Vertex);
			const size_t nSize = m_vVertexes.size() * nVertexSize;
			m_spImpl->update(&m_vVertexes[0], nSize, nVertexSize);
			m_bChanged = true;
		}

		const math::AABoxf& getBBox()
		{
			if ( m_bChanged )
			{

				for ( int i = 0; i < m_vVertexes.size(); ++i )
				{
					math::Point3f point = m_vVertexes[ i ].position ;
					math::extendVolume( m_AABB, point );
				}
				math::Vec3f cent = (m_AABB.getMin()+m_AABB.getMax())*0.5f;
				m_Sphere.setCenter( cent );
				m_Sphere.setRadius( math::length( cent ) );
				m_bChanged = false;
			}
			return m_AABB;
		}

		const math::Spheref& getBSphere()
		{
			if ( m_bChanged )
			{
				for ( int i = 0; i < m_vVertexes.size(); ++i )
				{
					math::Point3f point = m_vVertexes[ i ].position ;
					math::extendVolume( m_AABB, point );
				}
				math::Vec3f cent = (m_AABB.getMin()+m_AABB.getMax())*0.5f;
				m_Sphere.setCenter( cent );
				m_Sphere.setRadius( math::length( cent ) );
				m_bChanged = false;
			}
			return m_Sphere;
		}

	private:
		bool						m_bIsDynamic;
		Vertexes					m_vVertexes;
		std::auto_ptr<IGeometry>	m_spImpl;
		math::AABoxf				m_AABB;
		math::Spheref				m_Sphere;
		bool						m_bChanged;
	};


	class IIndexedGeometry
	{
	public:
		virtual ~IIndexedGeometry(){}
		static IIndexedGeometry* create(const vertex::VertexDecl decl, bool bUse32bitIndixes);
		//static IIndexedGeometry* create(std::wstring xml_filename);

		//static IIndexedGeometry* create(TiXmlNode* root_geom_node);
		virtual void updateVB(const void* pData, size_t nBytes, size_t size_of_vertex) = 0;
		virtual void updateIB(const void* pData, size_t nBytes) = 0;
		virtual void render(PrimitiveType ePrimType, unsigned nBaseVertexIndex, 
							unsigned nMinIndex, unsigned nNumVertices, 
							unsigned nStartIndex, unsigned nPrimitiveCount) = 0;
	};

	template<class Vertex, bool Use32Indexes>
	class TIndexedGeometry
	{
	};

	template< typename VertexType>
	class XmlVertexReader
	{
	public:
		static void Read( TiXmlElement*, VertexType& );
	};

#define START_VERTEX_READER(type) \
	template<> class XmlVertexReader<type>\
	{\
	public:\
	static void Read(TiXmlElement*, type&);\
	};\
	\
	inline void XmlVertexReader<type>::Read(TiXmlElement* pNode, type& vertex)\
	{
#define END_VERTEX_READER	}

	void readColor( TiXmlElement* pNode, math::Color& color);
	void readPosition( TiXmlElement* pNode, math::Vec3f& position);
	void readTangent( TiXmlElement* pNode, math::Vec3f& tangent);
	void readBinormal( TiXmlElement* pNode, math::Vec3f& binormal);
	void readNormal( TiXmlElement* pNode, math::Vec3f& normal);
	void readTexCoords( TiXmlElement* pNode, math::Vec2f& tex);
	void readTexCoords2( TiXmlElement* pNode, math::Vec2f& tex0, math::Vec2f& tex1);
	void readWeights( TiXmlElement* pNode, math::Vec4f& weights);


	#define READ_COLOR		readColor(pNode, vertex.color);
	#define READ_POSITION	readPosition(pNode, vertex.position);
	#define READ_TANGENT	readTangent(pNode, vertex.tangent);
	#define READ_BINORMAL	readBinormal(pNode, vertex.binormal);
	#define READ_NORMAL		readNormal(pNode, vertex.normal);
	#define READ_TEXCOORD	readTexCoords(pNode, vertex.tex);
	#define READ_TEXCOORD2	readTexCoords2(pNode, vertex.tex0, vertex.tex1 );
	#define READ_WEIGHTS	readWeights(pNode, vertex.weights);

	START_VERTEX_READER( vertex::PositionNormalColoredTexturedBinormalTangent )
		READ_POSITION
		READ_NORMAL
		READ_COLOR
		READ_TANGENT
		READ_BINORMAL
		READ_TEXCOORD
	END_VERTEX_READER

	START_VERTEX_READER( vertex::PositionNormalTextured2TangentBinorm )
		READ_POSITION
		READ_NORMAL
		READ_TEXCOORD2
		READ_TANGENT
		READ_BINORMAL		
	END_VERTEX_READER

	START_VERTEX_READER( vertex::PositionSkinnedNormalColoredTextured2TangentBinorm )
		READ_POSITION
		READ_NORMAL
		READ_COLOR
		READ_TANGENT
		READ_BINORMAL
		READ_TEXCOORD2
		READ_WEIGHTS
	END_VERTEX_READER

	template<typename IndexType>
	void readIBFromXml(TiXmlNode* root_geom_node, std::vector<IndexType>& ib)
	{
		if (0 == root_geom_node) return;

		TiXmlElement *elem = root_geom_node->FirstChildElement("faces");		

		int ninds;
		elem->Attribute("num", &ninds);           
		ib.reserve(ninds * 3);

		for(TiXmlElement* ev = elem->FirstChildElement("face");
			ev != NULL; ev = ev->NextSiblingElement("face")) 
		{		
			int x,y,z;

			ev->Attribute("a", &x);
			ev->Attribute("b", &y);
			ev->Attribute("c", &z);

			ib.push_back((unsigned int)x);
			ib.push_back((unsigned int)y);
			ib.push_back((unsigned int)z);
		}
	}

	template<typename VertexType>
	void readVBFromXml(TiXmlNode* root_geom_node, std::vector<VertexType>& vb)
	{
		TiXmlElement* elem = root_geom_node->FirstChildElement("vertices");
		if (0 == elem) return;
		
		TiXmlElement* ev = elem->FirstChildElement("vertex");
		if (0 == ev) return;

		int vertex_count;
		elem->Attribute( "num", &vertex_count ); 
		vb.resize( vertex_count );

		for ( std::vector<VertexType>::iterator vi = vb.begin(); vi != vb.end() && 0 != ev; ++vi)
		{
			XmlVertexReader< VertexType >::Read( ev, (*vi) );
			ev = ev->NextSiblingElement("vertex");
		}
	}


	template<typename IndexType, typename VertexType>
		void loadGeomDataFromXmlEl( 
		TiXmlNode* root_geom_node, 
		std::vector<VertexType>& vb, 
		std::vector<IndexType>& ib
		)
	{
		readIBFromXml(root_geom_node, ib);
		readVBFromXml(root_geom_node, vb);
	}

	template<typename IndexType, typename VertexType>
		void loadGeomDataFromXmlFile( 
				const std::string& xml_filename,
				std::vector<VertexType>& vb, 
				std::vector<IndexType>& ib
				)
	{
		std::vector<byte> data;
		io::CFileSystem& fs = io::TheFileSystem::get();
		io::readstream_ptr in = fs.findFile(xml_filename);
		if (in && in->getSize() > 0)
		{
			unsigned int size = in->getSize();
			io::stream_to_vector(data, in);

			TiXmlDocument xml;//( xml_filename );
			xml.Parse((const char*)&(data[0]));
			//if ( !xml.LoadFile() ) return;

			loadGeomDataFromXmlEl( xml.FirstChild( "mesh" ), vb, ib );
		}
	}


	template<class Vertex>
	class TIndexedGeometry<Vertex, true>
	{
	public:
		typedef std::vector<Vertex>			Vertexes;
		typedef std::vector<unsigned int>	Indexes;

		TIndexedGeometry() : m_spImpl(IIndexedGeometry::create(Vertex::GetDecl(), true))
		{
			m_bChanged = false;
		}

		void render(PrimitiveType ePrimType, unsigned nBaseVertexIndex, 
			unsigned nMinIndex, unsigned nNumVertices, 
			unsigned nStartIndex, unsigned nPrimitiveCount)
		{
			m_spImpl->render(ePrimType, nBaseVertexIndex, nMinIndex, 
				nNumVertices, nStartIndex, nPrimitiveCount); //TODO
		}

		void render(PrimitiveType ePrimType, unsigned nPrimitiveCount)
		{
			unsigned int nNumVertices = (unsigned int)m_vVertexes.size();
			Device::get().addStatistic(nNumVertices, nPrimitiveCount);
			m_spImpl->render(ePrimType, 0, 0, nNumVertices, 0, nPrimitiveCount);
		}

		void render(PrimitiveType ePrimType, unsigned nStartPrimitive, unsigned nPrimitiveCount)
		{
			//TODO for other primitive types
			unsigned int nNumVertices = (unsigned int)m_vVertexes.size();
			Device::get().addStatistic(nNumVertices, nPrimitiveCount-nStartPrimitive);
			m_spImpl->render(ePrimType, 0, 0, nPrimitiveCount*4, 6*nStartPrimitive, nPrimitiveCount );
		}

		void load( const std::string& xml_filename )
		{
			loadGeomDataFromXmlFile(xml_filename, m_vVertexes, m_vIndexes);

			unlockVB();
			unlockIB();

			updateBVolumes();
		}

		Vertexes& lockVB() {return m_vVertexes;}
		const Vertexes& getVB() const {return m_vVertexes;}

		void unlockVB()
		{
			const size_t nVertexSize = sizeof(Vertex);
			const size_t nSize = m_vVertexes.size() * nVertexSize;
			m_bChanged = true;
			m_spImpl->updateVB(&m_vVertexes[0], nSize, nVertexSize);
		}


		Indexes& lockIB()	{return m_vIndexes;}
		const Indexes& getIB() const {return m_vIndexes;}

		void unlockIB() 
		{
			m_spImpl->updateIB(&m_vIndexes[0], m_vIndexes.size()*sizeof(unsigned int);)
		}

		int getIndexNum() const					{ return (int)m_vIndexes.size(); }
		int getVertexNum() const				{ return (int)m_vVertexes.size(); }

		const math::AABoxf& getBBox() const		{ return m_AABB; }
		const math::Spheref& getBSphere() const { return m_Sphere; }

		void updateBVolumes()
		{
			if ( !m_bChanged ) return;

			size_t nNumVerts = m_vVertexes.size();
			for(size_t i = 0; i < nNumVerts; i++)
			{
				const math::Point3f& point = m_vVertexes[i].position;
				math::extendVolume(m_AABB, point);
			}
			math::Vec3f cent = (m_AABB.getMin()+m_AABB.getMax())*0.5f;
			m_Sphere.setCenter( cent );
			m_Sphere.setRadius( math::length( cent ) );
			m_bChanged = false;
		}

	private:
		void load( TiXmlNode* root_geom_node )
		{
			loadGeomDataFromXmlEl(root_geom_node, m_vIndexes, m_vVertexes);
		}

	private:
		Vertexes						m_vVertexes;
		std::auto_ptr<IIndexedGeometry>	m_spImpl;
		Indexes							m_vIndexes;
		math::AABoxf					m_AABB;
		math::Spheref					m_Sphere;
		bool							m_bChanged;
	};


	template<class Vertex>
	class TIndexedGeometry<Vertex, false>
	{
	public:
		typedef std::vector<Vertex>			Vertexes;
		typedef std::vector<unsigned short>	Indexes;

		TIndexedGeometry() 
			: m_spImpl(IIndexedGeometry::create(Vertex::GetDecl(), false))
		{			
			m_bChanged = false;
		}

		void render(PrimitiveType ePrimType, unsigned nBaseVertexIndex, 
					unsigned nMinIndex, unsigned nNumVertices, 
					unsigned nStartIndex, unsigned nPrimitiveCount)
		{
			m_spImpl->render(ePrimType, nBaseVertexIndex, nMinIndex, 
				nNumVertices, nStartIndex, nPrimitiveCount); //TODO
		}

		void render(PrimitiveType ePrimType, unsigned nPrimitiveCount)
		{
			unsigned int nNumVertices = (unsigned int)m_vVertexes.size();
			m_spImpl->render(ePrimType, 0, 0, nNumVertices, 0, nPrimitiveCount);
		}

		void render(PrimitiveType ePrimType, unsigned nStartPrimitive, unsigned nPrimitiveCount)
		{
			//TODO for other primitive types
			m_spImpl->render(ePrimType, 0, 0, nPrimitiveCount*4, 6*nStartPrimitive, nPrimitiveCount );
		}

		void loadFromXML(const std::string& filename)
		{

		}

		void loadFromBinary(const std::string& filename)
		{

		}

		void load( const std::string& filename )
		{
			std::vector<byte> data;
			io::CFileSystem& fs = io::TheFileSystem::get();

			bool use_binary = false;

			//first try to find binary file
			std::string bin_filename = filename + ".mesh";
			io::readstream_ptr bin_in = fs.findFile(bin_filename);
			if (!bin_in)
			{
				bin_filename = io::helpers::getFileNameWithoutExtension(filename);
				bin_filename += ".mesh";
				bin_in = fs.findFile(bin_filename);
			}

			bool loaded = false;

			if (bin_in && bin_in->getSize() > 0)
			{
				uint vertex_size = 0;//Vertex size
				uint vdata_size = 0;//Vertex Data Size
				(*bin_in) >> vertex_size >> vdata_size;
				if (vertex_size == sizeof(Vertex))
				{					
					//Vertex Data
					m_vVertexes.resize(vdata_size/vertex_size);
					bin_in->readbuff((byte*)&(m_vVertexes[0]), vdata_size);

					(*bin_in) >> vertex_size >> vdata_size;
					if (vertex_size == sizeof(unsigned short))
					{
						m_vIndexes.resize(vdata_size/vertex_size);
						bin_in->readbuff((byte*)&(m_vIndexes[0]), vdata_size);
						loaded = true;
					}
				}
			}

			if (!loaded)
				loadGeomDataFromXmlFile(filename, m_vVertexes, m_vIndexes);

			unlockIB();
			unlockVB();

			updateBVolumes();
		}

		Vertexes& lockVB() {return m_vVertexes;}
		const Vertexes& getVB() const {return m_vVertexes;}

		void unlockVB()
		{
			if (!m_vVertexes.empty())
			{
				const size_t nVertexSize = sizeof(Vertex);
				const size_t nSize = m_vVertexes.size() * nVertexSize;
				m_spImpl->updateVB(&m_vVertexes[0], nSize, nVertexSize);
				m_bChanged = true;
			}
		}

		Indexes& lockIB(){return m_vIndexes;}
		const Indexes& getIB() const {return m_vIndexes;}

		void unlockIB() 
		{
			if (!m_vIndexes.empty())
				m_spImpl->updateIB(&m_vIndexes[0], m_vIndexes.size()*sizeof(unsigned short));
		}

		int getIndexNum() const					{ return (int)m_vIndexes.size(); }
		int getVertexNum() const				{ return (int)m_vVertexes.size(); }

		const math::AABoxf& getBBox() const		{ return m_AABB; }
		const math::Spheref& getBSphere() const { return m_Sphere; }

		void updateBVolumes()
		{
			if ( !m_bChanged ) return;

			size_t nNumVerts = m_vVertexes.size();
			for(size_t i = 0; i < nNumVerts; i++)
			{
				//TODO: need more effective solution!
				math::Point3f point(m_vVertexes[i].position[0], 
					m_vVertexes[i].position[1], 
					m_vVertexes[i].position[2]);

				math::extendVolume(m_AABB, point);
			}
			math::Vec3f cent = (m_AABB.getMin()+m_AABB.getMax())*0.5f;
			m_Sphere.setCenter( cent );
			m_Sphere.setRadius( math::length( cent ) );
			m_bChanged = false;
		}

	private:
		void load( TiXmlNode* root_geom_node )
		{
			loadGeomDataFromXmlEl(root_geom_node, m_vVertexes, m_vIndexes);
		}

	private:
		Vertexes						m_vVertexes;
		std::auto_ptr<IIndexedGeometry>	m_spImpl;
		Indexes							m_vIndexes;
		math::AABoxf					m_AABB;
		math::Spheref					m_Sphere;
		bool							m_bChanged;
	};
}