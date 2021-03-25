/*
   Copyright 2021 Daniel S. Buckstein

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	GPRO Net SDK: Networking framework.
	By Daniel S. Buckstein

	gpro-net-RakNet.hpp
	Header for RakNet common management.
*/

#ifndef _GPRO_NET_RAKNET_HPP_
#define _GPRO_NET_RAKNET_HPP_
#ifdef __cplusplus


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "RakNet/RakPeerInterface.h"
#include "RakNet/MessageIdentifiers.h"
#include "RakNet/BitStream.h"
#include "RakNet/RakNetTypes.h"
#include "RakNet/GetTime.h"


namespace gproNet
{
	// eSettings
	//	Enumeration of common settings.
	enum eSettings
	{
		SET_GPRO_SERVER_PORT = 7777,
		SET_GPRO_MAX_CLIENTS = 128,
		SET_GPRO_AGENTS_PER_CLIENT = 16,
		SET_GPRO_MAX_AGENTS = SET_GPRO_MAX_CLIENTS * SET_GPRO_AGENTS_PER_CLIENT,	//128
	};


	// eMessageCommon
	//	Enumeration of custom common message identifiers.
	enum eMessageCommon
	{
		ID_GPRO_MESSAGE_COMMON_BEGIN = ID_USER_PACKET_ENUM,



		ID_GPRO_MESSAGE_COMMON_END
	};


	// sSimulationAgent
	// Boids agent.
	struct sSimulationAgent
	{
		short ownerID, agentID;
		float position[3];	// current state
		float velocity[3];	// integrates into position
		float acceleration[3];	// integrates into velocity
		float force[3];	//total of cohesion, alignment, separation
		
		static RakNet::BitStream& Write(RakNet::BitStream& bitstream, sSimulationAgent const& obj)
			//RakNet::BitStream& Write(RakNet::BitStream& bitstream);
		{

			// done
			return bitstream;
		}
		static RakNet::BitStream& Read(RakNet::BitStream& bitstream, sSimulationAgent& obj)
			//RakNet::BitStream& Read(RakNet::BitStream& bitstream);
		{

			//done
			return bitstream;
		}
	};

	// cRakNetManager
	//	Base class for RakNet peer management.
	class cRakNetManager abstract
	{
		// protected data
	protected:
		// peer
		//	Pointer to RakNet peer instance.
		RakNet::RakPeerInterface* peer;

		//agents
		//array of agents in the simulation
		// 2D array() : 
		// major index = client id [0,8)
		// minor index = agent id [0,16)
		sSimulationAgent agents[SET_GPRO_MAX_CLIENTS][SET_GPRO_AGENTS_PER_CLIENT];
		// 1D array ()
		// our agent range = [ i * 16, (i+1) * 16)
	//	sSimulationAgent agents[SET_GPRO_MAX_CLIENTS];

		// protected methods
	protected:
		// cRakNetManager
		//	Default constructor.
		cRakNetManager();

		// ~cRakNetManager
		//	Destructor.
		virtual ~cRakNetManager();

		// ProcessMessage
		//	Unpack and process packet message.
		//		param bitstream: packet data in bitstream
		//		param dtSendToReceive: locally-adjusted time difference from sender to receiver
		//		param msgID: message identifier
		//		return: was message processed
		virtual bool ProcessMessage(RakNet::BitStream& bitstream, RakNet::SystemAddress const sender, RakNet::Time const dtSendToReceive, RakNet::MessageID const msgID);

		// WriteTimestamp
		//	Write timestamp ID and current time.
		//		param bitstream: packet data in bitstream
		//		return: bitstream
		RakNet::BitStream& WriteTimestamp(RakNet::BitStream& bitstream);

		// ReadTimestamp
		//	Read timestamp ID and current time.
		//		param bitstream: packet data in bitstream
		//		return: bitstream
		RakNet::BitStream& ReadTimestamp(RakNet::BitStream& bitstream, RakNet::Time& dtSendToReceive_out, RakNet::MessageID& msgID_out);

		// WriteTest
		//	Write test greeting message.
		//		param bitstream: packet data in bitstream
		//		param message: message string
		//		return: bitstream
		RakNet::BitStream& WriteTest(RakNet::BitStream& bitstream, char const message[]);

		// ReadTest
		//	Read test greeting message.
		//		param bitstream: packet data in bitstream
		//		return: bitstream
		RakNet::BitStream& ReadTest(RakNet::BitStream& bitstream);

		// public methods
	public:
		// MessageLoop
		//	Unpack and process packets.
		//		return: number of messages processed
		int MessageLoop();
	};

}


#endif	// __cplusplus
#endif	// !_GPRO_NET_RAKNET_HPP_