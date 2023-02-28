#include "RoboCatPCH.h"
#include "NetworkManager.h"
#include <iostream>

/// <summary>
/// Initialize listenSocket so it can receive incoming connections.
/// listenSocket must not block on any future calls.
/// </summary>
void NetworkManager::Init()
{
	listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	
	SocketAddressPtr address = SocketAddressFactory::CreateIPv4FromString("127.0.0.1");

	int result = 0;
	result = listenSocket->Bind(*address);
	if (result != 0) return;

	std::cout << "Socket Bound" << std::endl;

	listenSocket->SetNonBlockingMode(true);

	result = listenSocket->Listen();
	if (result != 0) return;
}

/// <summary>
/// Called once per frame. Should check listenSocket for new connections.
/// If any come in, should add them to the list of openConnections, and
/// should send a message to each peer listing the addresses
/// of every other peer.
/// 
/// If there is a new connection, log some info about it with
/// messageLog.AddMessage().
/// </summary>
void NetworkManager::CheckForNewConnections()
{
	SocketAddress newConnection;
	TCPSocketPtr connectionSocket = listenSocket->Accept(newConnection);
	if (connectionSocket == nullptr)
	{
		return;
	}

	openConnections[newConnection] = connectionSocket;
}

/// <summary>
/// Sends the provided message to every connected peer. Called
/// whenever the user presses enter.
/// </summary>
/// <param name="message">Message to send</param>
void NetworkManager::SendMessageToPeers(const std::string& message)
{
	for (auto i : openConnections)
	{
		i.second->Send(&message, message.length());
	}
}

void NetworkManager::PostMessagesFromPeers()
{
	const size_t BUFLEN = 4096;
	char buffer[BUFLEN];

	for (auto i : openConnections)
	{
		i.second->Receive(buffer, BUFLEN);

		std::string message(buffer, BUFLEN);
		messageLog.AddMessage(message);
	}
}

/// <summary>
/// Try to connect to the given address.
/// </summary>
/// <param name="targetAddress">The address to try to connect to.</param>
void NetworkManager::AttemptToConnect(SocketAddressPtr targetAddress)
{
	listenSocket->Connect(*targetAddress);
}
