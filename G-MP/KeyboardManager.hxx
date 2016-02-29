#include <array>
#include <functional>
#include <boost/circular_buffer.hpp>
#include <vector>
#include <concurrent_queue.h>

class KeyboardManager
{
private:
	struct KeyData
	{
		bool PRESSED;
		bool RELEASED;
		bool DOWN;
	};
	std::array<KeyData, 0x100> Keys;
	void ResetKeyData();
	std::function<void(bool, unsigned char)> onKeyStateChange;
	boost::circular_buffer<unsigned char> key_buffer;
public:
	KeyboardManager(const std::function<void(bool, unsigned char)>& onKeyStateChange = nullptr);
	void CheckKeys(unsigned char key, bool pressed);
	bool Pressed(unsigned char key);
	bool Released(unsigned char key);
	bool Up(unsigned char key);
	bool Down(unsigned char key);
	bool BufferContainsArray(const std::vector<unsigned char>& compare);
	const std::vector<unsigned char> GetSequence(size_t size, bool removelast);
	void ClearBuffer();
	void SetOnKeyStateChangeFunction(const std::function<void(bool, unsigned char)>& onKeyStateChange);

	using CharacterRange = std::pair<unsigned char, unsigned char>;

	//predefined ranges
	const CharacterRange A2Z = CharacterRange('A', 'Z');
	const CharacterRange DIGITS = CharacterRange('0', '9');
	const CharacterRange DOT = CharacterRange('.', '.');
	const CharacterRange DBLPNT = CharacterRange(':', ':');

	const CharacterRange ALL = CharacterRange(0, 255);
	const CharacterRange _7BIT = CharacterRange(0, 127);
	const CharacterRange SECTOR_A = CharacterRange(' ', '/');
	const CharacterRange SECTOR_B = CharacterRange(':', '@');
	const CharacterRange SECTOR_C = CharacterRange('[', '`');

	std::vector<CharacterRange> AcceptCharacters;

	using TACS = std::pair<unsigned char, bool>;
	 
	Concurrency::concurrent_queue<TACS> AntiCrashSychronization;

	bool BackspaceTextEditing;
};

void KeyboardHandlerFunction(DWORD key, WORD repeats, BYTE scanCode, BOOL isExtended, BOOL isWithAlt, BOOL wasDownBefore, BOOL isUpNow);

extern KeyboardManager kbmgr;