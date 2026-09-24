#pragma once
#include <Includes/Includes.hpp>
#include <Includes/Utils.hpp>
#include <Core/Offsets.hpp>
#include <Core/Core.hpp>

#include <string>

namespace Core
{
	namespace Threads
	{
		class cUpdateNames
		{
		public:
			std::unordered_map<int, Core::SDK::Game::NetworkInfo> NetworkMap;

		public:
			void GetPlayerNames( )
			{
				// Network data collection removed
			}

			void Update( )
			{
				while ( true )
				{
					std::this_thread::sleep_for( std::chrono::milliseconds( 6000 ) );
					try {
						GetPlayerNames( );
					}
					catch ( ... ) { }
				}
			}
		};

		inline cUpdateNames g_UpdateNames;
	}
}
