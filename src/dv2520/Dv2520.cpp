#include <stdafx.h>

#include <Dx.h>
#include <Win.h>
#include <Cam.h>
#include <Dv2520.h>

Dv2520::Dv2520( Win& p_win ) {
	m_win = &p_win;

	float aspect = ( (float)m_win->getWidth() ) / ( (float)m_win->getHeight() );
	float fov = (float)RADIAN( 45.0f );
	float zFar = 1000;
	float zNear = 1;
	m_cam = new Cam( fov, aspect, zNear, zFar );

	m_dx = nullptr;
}
Dv2520::~Dv2520() {
	ASSERT_DELETE( m_dx );
	ASSERT_DELETE( m_cam );
}

HRESULT Dv2520::init() {
	HRESULT hr = S_FALSE;

	m_dx = new Dx( *m_win );
	hr = m_dx->init();

	return hr;
}

int Dv2520::run() {
	m_timerDelta.reset();
	m_timerDelta.start();

	MSG msgWin = { 0 };
	while( WM_QUIT!=msgWin.message ) {
		if( PeekMessage( &msgWin, NULL, 0, 0, PM_REMOVE ) ) {
			TranslateMessage( &msgWin );
			DispatchMessage( &msgWin );
		} else {
			double delta = m_timerDelta.tick();
			gameloop( delta );
		}
	}
	return (int)msgWin.wParam;
}

void Dv2520::gameloop( double p_delta ) {
	InputQueue inputQueue = m_win->getInputQueue();

#define TempWalkVelocity 0.5f
#define TempRotateVelcoty 0.1f

	InputKey key;
	while( inputQueue.keyPop( key )==false ) {
		switch( key.getType() ) {
		case InputKeyTypes_W:
			m_cam->walk( TempWalkVelocity );
			break;
		case InputKeyTypes_A:
			m_cam->strafe( -TempWalkVelocity/2 );
			break;
		case InputKeyTypes_S:
			m_cam->walk( -TempWalkVelocity );
			break;
		case InputKeyTypes_D:
			m_cam->strafe( TempWalkVelocity/2 );
			break;

		// Because I can't be bothered.
		case InputKeyTypes_UP:
			m_cam->pitch( -TempRotateVelcoty );
			break;
		case InputKeyTypes_LEFT:
			m_cam->yaw( -TempRotateVelcoty );
			break;
		case InputKeyTypes_DOWN:
			m_cam->pitch( TempRotateVelcoty );
			break;
		case InputKeyTypes_RIGHT:
			m_cam->yaw( TempRotateVelcoty );
			break;
		}
	}
	inputQueue.empty(); // Remember to clear the input.
	
	m_cam->update( p_delta );

	HRESULT hr = m_dx->render( p_delta, m_cam->getView(), m_cam->getProj() );
	if( FAILED( hr ) ) {
		ERR_HR( hr );
	}
}