/*	<channelmanager.hpp>
	Copyright(C) 2013,2014  Jan Simon Bunten
							Simon Kadel
							Martin Sven Oehler
							Arne Sven Stühlmeyer

	This File is part of the WhisperLibrary

	WhisperLibrary is free software : you can redistribute it and / or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	WhisperLibrary is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CHANNEL_MANAGER
#define CHANNEL_MANAGER

#include <vector>
#include "../../src/tcppacket.hpp"
#include <iostream>
#include <functional>
#include <genericpacket.hpp>
#include "../../src/udppacket.hpp"
#include <map>
#include "../../src/networkconnector.hpp"
#include "../../src/tcppacketgenerator.hpp"

namespace whisper_library {

class CovertChannel;

/**
	\brief The connecting part for the CovertChannel and the framework. 
	
	You can add and remove covert channels you want to use. 
	You pass the data you want to send to this class.
	It is connected to the network via a SocketConnector.
	
*/
class ChannelManager {

public:
	/** \brief Constructor
	*/
	ChannelManager();
	/** \brief Destructor
	*/
	~ChannelManager();

	/** \brief Sets the stream that is used to output error messages of the channelmanager
		\param stream stream that is used to output error messages
	*/
	void setErrorStream(std::ostream* stream);

	/** \brief Returns a list with available covert channels
		\return Returns a vector with the names of all available covert channels
	*/
	vector<string> getChannelNames();
	/** \brief Returns a list with descriptions of all available covert channels

		The order matches the name list retrieved by calling 'getChannelNames'.
		\return Returns a vector with descriptions of all covert channels
	*/
	vector<string> getChannelInfos();
	/** \brief Returns the amount of available channels
		\return Number of available channels
	*/
	unsigned int channelCount();
	/** \brief Sets the arguments of the current covert channel.

		The string arguments is parsed by the channel to set channel specific options.
		More information on which arguments are supported can be found in the specific covertchannel source.
		\param ip destination ip the covert channel is used to communicate with
		\param arguments argument string that is given to the channel
	*/
	void setChannelArguments(string ip, string arguments);

	// Callbacks for Covert Channels

	/** 
	  \brief Creates a basic tcp packet with http content
	  \param ip destination ip the packet is send to
	  \return valid tcp packet
	 */
	TcpPacket getTcpPacket(string ip);
	/** \brief Creates a basic udp packet
		\param port the port that the udp packet is send to
		\return Returns a valid udp packet
	*/
	UdpPacket getUdpPacket(unsigned short port);
	/** \brief creates a valid udp packet with given length
		\param port the port that the udp packet is send to
		\param length the desired length
	*/
	UdpPacket getUdpPacketWithLength(unsigned short port, int length);

	/**
	* \brief Sets the stream, that the covert channel uses as the output for received messages
	* \param stream a pointer to the output stream
	*/
	void setOutputStream(std::ostream* stream);
	/** \brief Sets a function that is called, when a covert channel receives a message
		\param message_callback Pointer to a function that is called, when a message is received.
		                        The first argument is the ip(v4) in dotted form, the message was received from. The second
								argument is the received message.
	*/
	void setMessageCallback(function<void(string, string)> message_callback);
	/** \brief Writes a message to the selected output stream.

		Change the output stream by calling 'setOutputStream'.
		\param ip the sender ip of the message
		\param message the message to be written
	*/
	void outputMessage(string ip, string message);

	/** \brief Is called, when a packet is received on the network
		
		It is then forwarded to the channel with the specified ip.
		\param ip Ip of the sender of the packet
		\param packet the packet that was received
	*/
	void packetReceived(string ip, GenericPacket packet);

	// Connection
	/** \brief Starts a conversation.

		Opens a connection to a specific ip using the covert channel that
		matches the id. After calling this, you can send messages to that ip by
		calling 'sendMessage'.
		\param ip Ipv4 you want to communicate with
		\param channel_id id of the channel you want to use, to hide the coversation.
		                  The id is the index of the channel name in the vector that 
						  is returned by calling 'getChannelNames'.
	*/
	bool openConnection(string ip, unsigned int channel_id);
	/** \brief Stops a conversation

		Closes the covert channel and stops listening for packets of the given ip.
		\param ip Ipv4 address of the host you no longer want to communicate with
	*/
	void closeConnection(string ip);
	/** \brief Returns the number of open connections
	*/
	unsigned int connectionCount();
	/** \brief Checks for an open connection with a specific ip
		\param ip Ipv4 address of the host that you want to check
		\return True, if there is an open connection, otherwise false
	*/
	bool connection(string ip);
	/** \brief Sends a message to the given ip

		There needs to be an open connection to be able to send a message to that ip.
		\param ip Ipv4 adress that you want to send the message to
	    \param message the message as string
	*/
	void sendMessage(string ip, string message);

	// Adapter handling
	/** \brief Sets the network adapter that is used to send (win32 only)
			   and receive messages.
		\param name unique name of the adapter. A list of adapters can be
		retrieved by calling 'networkAdapters'.
	*/
	void setAdapter(string name);
	/** \brief Returns the number of available network adapters.
	*/
	unsigned int adapterCount();
	/** \brief Returns a list of available network adapters.

		The name of a network adapter uniquely identifies it
		and is used to select it by calling 'setAdapter'.
		\return Vector with the unique names
	*/
	vector<string> networkAdapters();
	/** \brief Returns the description of an adapter
		\param adapter_name unique name of the adapter. Can be retrieved by calling 'networkAdapters'.
		\return adapter description as a string. If the description is NULL, an empty string is returned.
	*/
	string adapterDescription(string adapter_name);
	/** \brief Returns the network addresses of the currently set adapter
		\return Vector of Ipv4 addresses in dotted form.
	*/
	vector<string> adapterAddresses();
	
private:
	/** \brief Outputs an error message to the stream set with 'setErrorStream'
		\param message error message that is display to the user
	*/
	void outputErrorMessage(string message);
	/** \brief Covert channel factory

		Creates a covert channel instance and returns a pointer to the object. Add an entry to this function
		if you want to add your own covert channel. Don't forget to update the constant 'CHANNEL_COUNT'.
		\param ip Ipv4 address you want to communicate with using the created channel
		\param channel_id id of the channel, which matches the index in the vector returned by 'getChannelNames'
		\return Pointer to the created covert channel instance
	*/
	CovertChannel* createChannel(string ip, unsigned int channel_id);

<<<<<<< HEAD
	// hold all available channels, just to pull infos
	std::vector<CovertChannel*> m_channels;
	// stream that holds received messages
	std::ostream* m_output_stream;
	// stream that displays errors
	std::ostream* m_error_stream;

	NetworkConnector* m_network;
	map<string, CovertChannel*> m_ip_mapping;
	map<string, TcpPacketGenerator*> m_generator_mapping;
	const unsigned int CHANNEL_COUNT;
	function<void(string, string)> m_message_callback;
=======
	std::vector<CovertChannel*> m_channels; /**< Holds an instance of each available covert channel type.
											   Used just to pull information from, not for communication. */
	std::ostream* m_output_stream; ///< Stream that is used to output messages received by a covert channel
	std::ostream* m_error_stream; ///< Stream that is used to output error messages
	NetworkConnector* m_network; ///< Pointer to NetworkConnector that is used to access network functionalities
	map<string, CovertChannel*> m_ip_mapping; /**< Maps Ipv4 addresses in dotted form to a pointer of the covert 
											  channel that is used to communicate with that ip */
	const unsigned int CHANNEL_COUNT; ///< Number of available covert channels. Needs to be updated if a new channel is added.
	function<void(string, string)> m_message_callback; /**< Pointer to the function that is called 
													   when a covert channel receives a message. 
													   The first argument is the ip(v4) in dotted form, 
													   the message was received from. The second argument is 
													   the received message.*/	
>>>>>>> master
};
}
#endif // CHANNEL_MANAGER
