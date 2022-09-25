 #include <Arduino.h>


    class PrintToString : public Print
    {
        private:
            String* data;

        public:
            PrintToString();
            ~PrintToString();

            String* results();
            void clear();


            size_t write(uint8_t buff);
            size_t write(const uint8_t* buffer, size_t size);

    };