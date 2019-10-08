#ifndef console_header_guard
#define console_header_guard

namespace console_class {

	class Console {
		public:

		void setConsoleColor(BYTE attribute);
		void setConsoleColor();

		void restoreOriginalAttributes();
		void restorePreviousAttributes();

		void show(bool no_close = false);
		void hide();
		void close();

		void setFont(std::string font_face_name, SHORT size = 12, UINT weight = FW_NORMAL);
		void setTitle(std::string text);

		WORD text_attribute = 0;

		///////////////////////////////////////////////////////////

		template <typename T>
		Console& operator<<(const T& x) {
			std::cout << x;

			return *this;
		}

		typedef Console& (*manipulator)(Console&);
		Console& operator<<(manipulator manip) {

			return manip(*this);
		}
		static Console& endl(Console& stream) {
			std::cout << std::endl;		

			return stream;
		}

		typedef std::basic_ostream<char, std::char_traits<char> > CoutType;
		typedef CoutType& (*StandardEndLine)(CoutType&);
		Console& operator<<(StandardEndLine manip) {
			manip(std::cout);

			return *this;
		}

		///////////////////////////////////////////////////////////

		private:
		WORD original_text_attributes = 0;
		WORD previous_text_attributes = 0;
		bool show_state = false;
		bool on_state = false;
	};

	namespace textColor {
		enum console_text_colors {
			black = 0x00,
			d_blue = 0x01,
			d_green = 0x02,
			teal = 0x03,
			d_red = 0x04,
			d_pink = 0x05,
			d_yellow = 0x06,
			default = 0x07,
			d_gray = 0x08,
			blue = 0x09,
			green = 0x0a,
			cyan = 0x0b,
			red = 0x0c,
			pink = 0x0d,
			yellow = 0x0e,
			white = 0x0f
		};
	}
	namespace backgroundColor {
		enum console_text_colors {
			black = 0x00,
			d_blue = 0x10,
			d_green = 0x20,
			teal = 0x30,
			d_red = 0x40,
			d_pink = 0x50,
			d_yellow = 0x60,
			gray = 0x70,
			d_gray = 0x80,
			blue = 0x90,
			green = 0xa0,
			cyan = 0xb0,
			red = 0xc0,
			pink = 0xd0,
			yellow = 0xe0,
			white = 0xf0
		};
	}

	namespace t = textColor;
	namespace b = backgroundColor;
}

#endif

