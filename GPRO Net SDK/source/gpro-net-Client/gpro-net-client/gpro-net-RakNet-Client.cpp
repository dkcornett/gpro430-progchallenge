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

	gpro-net-RakNet-Client.cpp
	Source for RakNet client management.
*/

#include <vector>
#include "gpro-net/gpro-net-client/gpro-net-RakNet-Client.hpp"


struct sSpatialPose
{
	float scale[3];     // non-uniform scale
	float rotate[3];    // orientation as Euler angles
	float translate[3]; // translation

	float spacialValues[9];		//all values of above arrays in one array
	float newSpacialValues[9];		//new version of SpacialValues to compare to last version	

	vector <int> changes;


	//when reading the changes vector also change the spacialValues array to the values of newSpatialValues, 
	//which will no longer be new once we get another packet with a more changes<>
	void ValuesCompare()
	{
		for (int i = 0; i < 9; i++)
		{
			if (spacialValues[i] != newSpacialValues[i])
			{
				changes.add(i);
				changes.add(newSpacialValues[i]);
			}

		}
	}
	
	//with new changes<>, change the data in newSpacialValues()
	void ChangeValues()
	{
		//for every 2 elements, take the value of the second and place into index of the first in spacialValues[]
		for (int i = 0; i < changes.length(); i += 2)
		{
			newSpacialValues[i] = changes.at(i + 1);

		}

		for (int i = 0; i < 9; i++)
		{
			spacialValues[i] = newSpacialValues[i];
		}

		UpdateLocationData();


	}

	//the function that actually changes location data for the struct
	//separate from ChangeValues() note the objects can be moved other ways than from data incoming from server
	void UpdateLocationData()
	{
		//change scale array
		scale[0] = spacialValues[0];
		scale[1] = spacialValues[1];
		scale[2] = spacialValues[2];
		//change rotation array
		rotate[0] = spacialValues[0];
		rotate[1] = spacialValues[1];
		rotate[2] = spacialValues[2];
		//change translation array
		translate[0] = spacialValues[0];
		translate[1] = spacialValues[1];
		translate[2] = spacialValues[2];

	}

	//for compression and decompresion we are clamping to a certain level of accuracy

	//THIS IS AN EXAMPLE OF THE SIMPLEST TYPE OF THIS CONVERSION

	//make the float input an int 
	int compress(float input)
	{
		int compression = int(input * 1000.0f);
		return compression;
	}

	//make the int input a float
	float decompress(int input)
	{
		float decompression = input * 0.001f;
		return decompression;
	}

	// read from stream
	RakNet::BitStream& Read(RakNet::BitStream& bitstream)
	{
		/*
		bitstream.Read(scale[0]);
		bitstream.Read(scale[1]);
		bitstream.Read(scale[2]);
		bitstream.Read(rotate[0]);
		bitstream.Read(rotate[1]);
		bitstream.Read(rotate[2]);
		bitstream.Read(translate[0]);
		bitstream.Read(translate[1]);
		bitstream.Read(translate[2]);*/

	
		bitstream.Read(changes);		//recieve list of changes
		
		ChangeValues();					//actually change the values according to the data we just took
	

		

		return bitstream;
	}

	// write to stream
	RakNet::BitStream& Write(RakNet::BitStream& bitstream) const
	{
	/*	bitstream.Write(scale[0]);
		bitstream.Write(scale[1]);
		bitstream.Write(scale[2]);
		bitstream.Write(rotate[0]);
		bitstream.Write(rotate[1]);
		bitstream.Write(rotate[2]);
		bitstream.Write(translate[0]);
		bitstream.Write(translate[1]);
		bitstream.Write(translate[2]);*/	//DO THIS PART OUTSIDE OF STRUCT, ONLY WRITE THE CHANGES TO BITSTREAM

		ValuesCompare();				//take differences between this location data and last pass, 
										//and put those values into the correct slots in changes<>
		
		bitstream.Write(changes);		//write the changes<> from this pass

		return bitstream;
	}
};

namespace gproNet
{
	cRakNetClient::cRakNetClient()
	{
		RakNet::SocketDescriptor sd;
		char SERVER_IP[16] = "127.0.0.1";

		peer->Startup(1, &sd, 1);
		peer->SetMaximumIncomingConnections(0);
		peer->Connect(SERVER_IP, SET_GPRO_SERVER_PORT, 0, 0);
	}

	cRakNetClient::~cRakNetClient()
	{
		peer->Shutdown(0);
	}

	bool cRakNetClient::ProcessMessage(RakNet::BitStream& bitstream, RakNet::SystemAddress const sender, RakNet::Time const dtSendToReceive, RakNet::MessageID const msgID)
	{
		if (cRakNetManager::ProcessMessage(bitstream, sender, dtSendToReceive, msgID))
			return true;

		// client-specific messages
		switch (msgID)
		{
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			//printf("Another client has disconnected.\n");
			break;
		case ID_REMOTE_CONNECTION_LOST:
			//printf("Another client has lost the connection.\n");
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			//printf("Another client has connected.\n");
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			//printf("The server is full.\n");
			return true;
		case ID_DISCONNECTION_NOTIFICATION:
			//printf("We have been disconnected.\n");
			return true;
		case ID_CONNECTION_LOST:
			//printf("Connection lost.\n");
			return true;

		case ID_CONNECTION_REQUEST_ACCEPTED:
		{
			// client connects to server, send greeting
			RakNet::BitStream bitstream_w;
			WriteTest(bitstream_w, "Hello server from client");
			peer->Send(&bitstream_w, MEDIUM_PRIORITY, UNRELIABLE_SEQUENCED, 0, sender, false);
		}	return true;

			// test message
		case ID_GPRO_MESSAGE_COMMON_BEGIN:
		{
			// client receives greeting, just print it
			ReadTest(bitstream);
		}	return true;

		}
		return false;
	}
}