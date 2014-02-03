// 	Author: Martin Oehler

#include <morseCoder.hpp>
#include <boost/algorithm/string.hpp>

namespace whisper_library {
	MorseCoder::MorseCoder(float delay_short, float delay_long, float delay_letter, float delay_space) :
		m_delay_short(delay_short),
		m_delay_long(delay_long),
		m_delay_letter(delay_letter),
		m_delay_space(delay_space) {

		m_morse_map.insert(morse('A', "01"));
		m_morse_map.insert(morse('B', "1000"));
		m_morse_map.insert(morse('C', "1010"));
		m_morse_map.insert(morse('D', "100"));
		m_morse_map.insert(morse('E', "0"));
		m_morse_map.insert(morse('F', "0010"));
		m_morse_map.insert(morse('G', "110"));
		m_morse_map.insert(morse('H', "0000"));
		m_morse_map.insert(morse('I', "00"));
		m_morse_map.insert(morse('J', "0111"));
		m_morse_map.insert(morse('K', "101"));
		m_morse_map.insert(morse('L', "0100"));
		m_morse_map.insert(morse('M', "11"));
		m_morse_map.insert(morse('N', "10"));
		m_morse_map.insert(morse('O', "111"));
		m_morse_map.insert(morse('P', "0110"));
		m_morse_map.insert(morse('Q', "1101"));
		m_morse_map.insert(morse('R', "010"));
		m_morse_map.insert(morse('S', "000"));
		m_morse_map.insert(morse('T', "1"));
		m_morse_map.insert(morse('U', "001"));
		m_morse_map.insert(morse('V', "0001"));
		m_morse_map.insert(morse('W', "011"));
		m_morse_map.insert(morse('X', "1001"));
		m_morse_map.insert(morse('Y', "1011"));
		m_morse_map.insert(morse('Z', "1100"));
		m_morse_map.insert(morse('0', "11111"));
		m_morse_map.insert(morse('1', "01111"));
		m_morse_map.insert(morse('2', "00111"));
		m_morse_map.insert(morse('3', "00011"));
		m_morse_map.insert(morse('4', "00001"));
		m_morse_map.insert(morse('5', "00000"));
		m_morse_map.insert(morse('6', "10000"));
		m_morse_map.insert(morse('7', "11000"));
		m_morse_map.insert(morse('8', "11100"));
		m_morse_map.insert(morse('9', "11110"));
		m_morse_map.insert(morse('.', "010101"));
		m_morse_map.insert(morse(',', "110011"));
		m_morse_map.insert(morse(':', "111000"));
		m_morse_map.insert(morse(';', "101010"));
		m_morse_map.insert(morse('?', "001100"));
		m_morse_map.insert(morse('-', "100001"));
		m_morse_map.insert(morse('_', "001101"));
		m_morse_map.insert(morse('(', "10110"));
		m_morse_map.insert(morse(')', "101101"));
		m_morse_map.insert(morse('\'', "011110"));
		//m_morse_map.insert(morse('=', "100001")); For some reason, '=' can't be used as a key. Can't fix that.
		m_morse_map.insert(morse('+', "01010"));
		m_morse_map.insert(morse('/', "10010"));
		m_morse_map.insert(morse('@', "011010"));
		m_morse_map.insert(morse('#', "000110")); // Addition for unsupported symbols
	}

	vector<float> MorseCoder::encodeMessage(string message) {
		// split message into words
		vector<string> words;
		boost::split(words, message, boost::is_any_of(" "), boost::token_compress_on);


		vector<float> delays;
		for (vector<string>::iterator it = words.begin(); it != words.end(); it++) {
			string word = (*it);
			// encode and push word
			for (unsigned int i = 0; i < word.length(); i++) {
				vector<bool> encoded_letter = encodeLetter(word[i]);
				//push letter
				for (vector<bool>::iterator it = encoded_letter.begin(); it != encoded_letter.end(); it++) {
					if ((*it)) {
						delays.push_back(m_delay_long);
					}
					else {
						delays.push_back(m_delay_short);
					}
				}
				// push short pause after letter, if not last of word
				if (i != word.length() - 1) {
					delays.push_back(m_delay_letter);
				}
			}
			// push long pause between words
			delays.push_back(m_delay_space);

		}

		return delays;
	}

	vector<char> MorseCoder::checkString(string message) {
		vector<char> unsupported_letters;
		for (int i = 0; i < message.length; i++) {
			try {
				m_morse_map.left.at(message[i]);
			}
			catch (const out_of_range& e) {
				unsupported_letters.push_back(message[i]);
			}
		}
		return unsupported_letters;
	}

	vector<bool>  MorseCoder::encodeLetter(char letter) {
		char uppercase_letter = toupper(letter);
		string encoding = "000110"; // #
		try {
			encoding = m_morse_map.left.at(uppercase_letter);
		}
		catch (const out_of_range& e) {
			cout << "Symbol " << uppercase_letter << " not supported." << e.what() << endl;
		}

		vector<bool> encoding_vector;
		for (unsigned int i = 0; i < encoding.length(); i++) {
			encoding_vector.push_back(encoding[i] == '1');
		}
		return encoding_vector;
	}



	string MorseCoder::decodeMessage(vector<float> delays) {
		string message = "";
		vector<bool> morse_code;
		for (vector<float>::iterator it = delays.begin(); it != delays.end(); it++) {
			float delay = (*it);
			if (delay == m_delay_short) {
				morse_code.push_back(false);
			}
			else {
				if (delay == m_delay_long) {
					morse_code.push_back(true);
				}
				else {
					char letter = decodeLetter(morse_code);
					message += letter;
					morse_code.clear();
					if (delay == m_delay_space && it != delays.end() -1) {
						message += " ";
					}
				}
			}
		}
		return message;
	}

	char MorseCoder::decodeLetter(vector<bool> morse_code) {
		char letter = '?';
		string morse_code_string = "";
		for (vector<bool>::iterator it = morse_code.begin(); it != morse_code.end(); it++) {
			if ((*it)) {
				morse_code_string += "1";
			}
			else {
				morse_code_string += "0";
			}
		}
		try {
			letter = m_morse_map.right.at(morse_code_string);
		}
		catch (const out_of_range& e) {
			cout << "Morse code not found." << e.what() << endl;
		}

		return letter;
	}
}