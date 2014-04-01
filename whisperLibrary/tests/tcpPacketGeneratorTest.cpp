#include <boost/test/unit_test.hpp>
#include "../src/tcppacketgenerator.hpp"
struct TcpPacketGeneratorFixture {
	TcpPacketGeneratorFixture() {
		sender = new whisper_library::TcpPacketGenerator(8080, std::bind(&TcpPacketGeneratorFixture::sendToReceiver, this, placeholders::_1),
															   std::bind(&TcpPacketGeneratorFixture::forward, this, placeholders::_1));
		receiver = new whisper_library::TcpPacketGenerator(8080, std::bind(&TcpPacketGeneratorFixture::sendToSender, this, placeholders::_1),
			std::bind(&TcpPacketGeneratorFixture::forward, this, placeholders::_1));
	}
	~TcpPacketGeneratorFixture() {
		delete sender;
		delete receiver;
	}

	void sendToSender(whisper_library::TcpPacket packet) {
		sender->receivePacket(packet);
	}

	void sendToReceiver(whisper_library::TcpPacket packet) {
		receiver->receivePacket(packet);
	}

	void forward(whisper_library::GenericPacket packet) {

	}

	whisper_library::TcpPacketGenerator* sender;
	whisper_library::TcpPacketGenerator* receiver;
};

BOOST_FIXTURE_TEST_SUITE(tcpPacketGenerator, TcpPacketGeneratorFixture);

BOOST_AUTO_TEST_CASE(tcp_handshake_test) {
	sender->sendConnect();
	BOOST_CHECK_EQUAL(sender->status(), whisper_library::TcpPacketGenerator::ESTABLISHED);
	BOOST_CHECK_EQUAL(receiver->status(), whisper_library::TcpPacketGenerator::ESTABLISHED);

	whisper_library::TcpPacket packet_first = sender->nextPacket();
	unsigned long first_sequence = packet_first.sequenceNumber();
	sendToReceiver(packet_first);
	whisper_library::TcpPacket packet_second = sender->nextPacket();
	unsigned long second_sequence = packet_second.sequenceNumber();
	BOOST_CHECK_LT(first_sequence, second_sequence);
}

BOOST_AUTO_TEST_SUITE_END()