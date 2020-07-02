#include "App.h"
#include "Box.h"
#include "Cylinder.h"
#include <memory>
#include <algorithm>
#include "MyMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"

namespace dx = DirectX;

GDIPlusManager gdipm;

App::App()
	:
	wnd( 800,600,"Point Light Simulation imGUI활용" ),
	light( wnd.Gfx() )
{
	class Factory
	{
	public:
		Factory( Graphics& gfx )
			:
			gfx( gfx )
		{}
		std::unique_ptr<Drawable> operator()()
		{
			const DirectX::XMFLOAT3 mat = { cdist( rng ),cdist( rng ),cdist( rng ) };

			switch( sdist( rng ) )
			{
			case 0:
				return std::make_unique<Box>(
					gfx,rng,adist,ddist,
					odist,rdist,bdist,mat
				);
			case 1:
				return std::make_unique<Cylinder>(
					gfx,rng,adist,ddist,odist,
					rdist,bdist,tdist
				);
			default:
				assert( false && "impossible drawable option in factory" );
				return {};
			}
		}
	private:
		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_int_distribution<int> sdist{ 0,1 };
		std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };//yaw
		std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };//pitch
		std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };//roll
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_real_distribution<float> cdist{ 0.0f,1.0f };
		std::uniform_int_distribution<int> tdist{ 3,30 };
	};

	drawables.reserve( nDrawables );
	std::generate_n( std::back_inserter( drawables ),nDrawables,Factory{ wnd.Gfx() } );//n개의 오브젝트들을 랜덤한 위치에 랜덤한 회전 값으로 생성한다.

	wnd.Gfx().SetProjection( dx::XMMatrixPerspectiveLH( 1.0f,3.0f / 4.0f,0.5f,40.0f ) );
}

void App::DoFrame()
{
	const auto dt = timer.GetDeltaTime() * speed_factor;
	wnd.Gfx().BeginFrame( 0.07f,0.0f,0.12f );
	wnd.Gfx().SetCamera( cam.GetMatrix() );
	light.Bind( wnd.Gfx(),cam.GetMatrix() );

	for( auto& d : drawables )
	{
		d->Update( wnd.kbd.KeyIsPressed( VK_SPACE ) ? 0.0f : dt );
		d->Draw( wnd.Gfx() );
	}
	light.Draw( wnd.Gfx() );

	// 시뮬레이션 스피드 조절을 위한 imgui
	if( ImGui::Begin( "Simulation Speed" ) )
	{
		ImGui::SliderFloat( "Speed Factor",&speed_factor,0.0f,6.0f,"%.4f",3.2f );
		ImGui::Text( "%.3f ms/frame (%.1f FPS)",1000.0f / ImGui::GetIO().Framerate,ImGui::GetIO().Framerate );
		ImGui::Text( "Status: %s",wnd.kbd.KeyIsPressed( VK_SPACE ) ? "PAUSED" : "RUNNING (hold spacebar to pause)" );
	}
	ImGui::End();
	// 카메라 컨트롤 윈도우 
	cam.SpawnControlWindow();
	// light 컨트롤 윈도우
	light.SpawnControlWindow();

	// 전프레임
	wnd.Gfx().EndFrame();
}

App::~App()
{}


int App::Go()
{
	while( true )
	{
		// process all messages pending, but to not block for new messages
		if( const auto ecode = Window::ProcessMessages() )
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
		DoFrame();
	}
}