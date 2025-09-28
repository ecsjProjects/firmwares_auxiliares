// Função para calcular CRC (permanece a mesma)
byte calcCRC(byte *data, byte len)
{
    byte crc = 0;
    for (int i = 0; i < len; i++)
    {
        crc ^= data[i];
    }
    return crc;
}
// Função para enviar um pacote
void sendPacket(byte command, byte *params, byte param_len)
{
    byte packet_len = 1 + param_len + 1 + 1 + 1; // COMMAND + PARAMETERS + CRC + ACK + END_BYTE
    // Construir o pacote completo em um buffer para envio eficiente
    byte packet[packet_len + 2]; // +2 para START_BYTE e packet_len
    packet[0] = START_BYTE;
    packet[1] = packet_len;
    packet[2] = command;
    for (int i = 0; i < param_len; i++)
    {
        packet[3 + i] = params[i];
    }
    // CRC calcula sobre COMMAND+PARAMETERS
    byte crc_data[1 + param_len];
    crc_data[0] = command;
    for (int i = 0; i < param_len; i++)
    {
        crc_data[1 + i] = params[i];
    }
    byte crc = calcCRC(crc_data, 1 + param_len);
    packet[3 + param_len] = crc;
    packet[3 + param_len + 1] = ACK_BYTE;
    packet[3 + param_len + 2] = END_BYTE;
    // Enviar o pacote completo
    SERCOM6_USART_Write(packet, packet_len + 2); // Tamanho total: START_BYTE + packet_len + conteúdo
}
// Função para receber um pacote
void receivePacket()
{
    static enum { WAIT_START,
                  READ_LEN,
                  READ_CMD,
                  READ_PARAMS,
                  READ_CRC,
                  READ_ACK,
                  WAIT_END } state = WAIT_START;
    static byte packet_len, command, params[10], param_len, crc, ack, bytes_read;
    while (SERCOM6_USART_ReadCountGet() > 0)
    {
        byte b;
        SERCOM6_USART_Read(&b, 1);
        switch (state)
        {
        case WAIT_START:
            if (b == START_BYTE)
                state = READ_LEN;
            break;
        case READ_LEN:
            packet_len = b;
            bytes_read = 0;
            state = READ_CMD;
            break;
        case READ_CMD:
            command = b;
            param_len = packet_len - (1 + 1 + 1 + 1); // COMMAND, CRC, ACK, END_BYTE
            bytes_read = 0;
            state = param_len > 0 ? READ_PARAMS : READ_CRC;
            break;
        case READ_PARAMS:
            params[bytes_read++] = b;
            if (bytes_read >= param_len)
                state = READ_CRC;
            break;
        case READ_CRC:
            crc = b;
            state = READ_ACK;
            break;
        case READ_ACK:
            ack = b;
            state = WAIT_END;
            break;
        case WAIT_END:
            if (b == END_BYTE)
            {
                // Valida CRC
                byte crc_data[1 + param_len];
                crc_data[0] = command;
                for (int i = 0; i < param_len; i++)
                    crc_data[1 + i] = params[i];
                byte calc_crc = calcCRC(crc_data, 1 + param_len);
                if (calc_crc == crc)
                {
                    // Pacote válido!
                    // Trate comando, parâmetros e ACK conforme sua lógica
                }
                else
                {
                    // CRC errado
                }
            }
            state = WAIT_START;
            break;
        }
    }
}