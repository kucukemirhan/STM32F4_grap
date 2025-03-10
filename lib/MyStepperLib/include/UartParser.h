#pragma once 

#include <UART_lib.h>
#include <cstdio>
#include <cstring>

#define JOINT_COUNT 4

class UartParser : public UartDMA {
public:
    UartParser(USART_TypeDef *Instance, UART_HandleTypeDef *huart);
    float getJointPosition(uint8_t j_index) const;

protected:
    static constexpr size_t MAX_J_VARS = JOINT_COUNT;

    void put(uint16_t index, uint16_t size) override;

private:
    static constexpr size_t PARSER_BUF_SIZE = 128;
    char parserBuf[PARSER_BUF_SIZE];
    size_t parserIndex; 
    float JointPositions[MAX_J_VARS];

    // Parses a complete line (e.g., "j1=334.2,j2=123.43,j3=12.43,j4=9.89").
    void parseLine(const char *line);

    // Parses an individual token (e.g., "j1=334.2") and stores the value.
    void parseToken(const char *token);
};

inline UartParser::UartParser(USART_TypeDef *Instance, UART_HandleTypeDef *huart) :
    UartDMA(Instance, huart), parserIndex(0)
{
    // Initialize all j_vars to 0.0f
    for (size_t i = 0; i < MAX_J_VARS; i++) {
        JointPositions[i] = 0.0f;
    }
}

inline float UartParser::getJointPosition(uint8_t j_index) const {
    if (j_index >= 1 && j_index <= MAX_J_VARS)
        return JointPositions[j_index - 1];
    else
        return 0.0f; // Alternatively, handle error as needed
}

inline void UartParser::put(uint16_t index, uint16_t size)
{
    // First, let UartDMA update its circular buffer (handling static variables, etc.).
    UartDMA::put(index, size);

    // Then, optimize by pulling all available bytes from _buffer at once.
    uint16_t avail = _buffer->size();
    if (avail > 0)
    {
        // Limit the number of bytes to avoid overrunning our parser buffer.
        if (avail > (PARSER_BUF_SIZE - 1))
            avail = PARSER_BUF_SIZE - 1;

        // Create a local temporary block to pull available bytes.
        uint8_t localBuf[PARSER_BUF_SIZE] = {0};
        uint16_t numPulled = _buffer->pull(localBuf, avail);

        // Process each character in the local block.
        for (uint16_t i = 0; i < numPulled; i++)
        {
            char c = static_cast<char>(localBuf[i]);
            // Newline or carriage return indicates end-of-line.
            if (c == '\n' || c == '\r')
            {
                parserBuf[parserIndex] = '\0';  // Null-terminate the accumulated line.
                parseLine(parserBuf);           // Parse the complete line.
                parserIndex = 0;                // Reset for the next line.
            }
            else
            {
                // Append character if space is available.
                if (parserIndex < (PARSER_BUF_SIZE - 1))
                    parserBuf[parserIndex++] = c;
                else
                    parserIndex = 0; // Overflow: reset the parser.
            }
        }
    }
}

inline void UartParser::parseLine(const char *line)
{
    // Copy the line to a temporary buffer so strtok can modify it.
    char tmp[PARSER_BUF_SIZE];
    strncpy(tmp, line, PARSER_BUF_SIZE);
    tmp[PARSER_BUF_SIZE - 1] = '\0';  // Ensure null termination.

    // Tokenize the line using comma as the delimiter.
    char *token = strtok(tmp, ",");
    while (token != nullptr)
    {
        parseToken(token);
        token = strtok(nullptr, ",");
    }
}

inline void UartParser::parseToken(const char *token)
{
    // Create a local copy to allow modification.
    char tokenCopy[64];
    strncpy(tokenCopy, token, sizeof(tokenCopy));
    tokenCopy[sizeof(tokenCopy) - 1] = '\0';

    // Trim trailing carriage return and newline characters.
    size_t len = strlen(tokenCopy);
    while (len > 0 && (tokenCopy[len - 1] == '\r' || tokenCopy[len - 1] == '\n'))
    {
        tokenCopy[len - 1] = '\0';
        len--;
    }

    // Expect the token in the format "j<index>=<value>"
    // Find the '=' character
    char *equalSign = strchr(tokenCopy, '=');
    if (equalSign == nullptr)
        return; // Format error

    // Get the index by skipping the 'j'
    int index = atoi(tokenCopy + 1);

    // Convert the float value using atof or strtof
    float value = static_cast<float>(atof(equalSign + 1));

    if (index >= 1 && index <= static_cast<int>(MAX_J_VARS))
        JointPositions[index - 1] = value;
}

