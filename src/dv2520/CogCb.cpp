#include <stdafx.h>

#include <CogCb.h>

CogCb::CogCb() {
	m_cbPerInstance = nullptr;
	m_cbPerFrame = nullptr;
	m_cbPerObject = nullptr;
}
CogCb::~CogCb() {
	ASSERT_DELETE( m_cbPerInstance );
	ASSERT_DELETE( m_cbPerFrame );
	ASSERT_DELETE( m_cbPerObject );
}

HRESULT CogCb::init( ID3D11Device* p_device ) {
	HRESULT hr = S_OK;

	m_cbPerInstance = new Cb< CbPerInstance >();
	hr = m_cbPerInstance->init( p_device );
	if( SUCCEEDED( hr ) ) {
		m_cbPerFrame = new Cb< CbPerFrame >();
		hr = m_cbPerFrame->init( p_device );
	}
	if( SUCCEEDED( hr ) ) {
		m_cbPerObject = new Cb< CbPerObject >();
		hr = m_cbPerObject->init( p_device );
	}

	return hr;
}

void CogCb::setCbs( ID3D11DeviceContext* p_devcon ) {
	ID3D11Buffer* cbs[] = {
		m_cbPerInstance->getBuf(),
		m_cbPerFrame->getBuf(),
		m_cbPerObject->getBuf()
	};

	p_devcon->CSSetConstantBuffers( 0, 3, cbs);
}

HRESULT CogCb::mapCbPerInstance( ID3D11DeviceContext* p_devcon, CbPerInstance& p_new ) {
	return mapCb( p_devcon, m_cbPerInstance, &p_new );
}
HRESULT CogCb::mapCbPerFrame( ID3D11DeviceContext* p_devcon, CbPerFrame& p_new ) {
	return mapCb( p_devcon, m_cbPerFrame, &p_new );
}
HRESULT CogCb::mapCbPerObject( ID3D11DeviceContext* p_devcon, CbPerObject& p_new ) {
	return mapCb( p_devcon, m_cbPerObject, &p_new );
}