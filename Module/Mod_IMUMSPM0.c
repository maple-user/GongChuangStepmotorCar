
#include "BSP_DWT.h"
#include "Mod_IMUMSPM0.h"

#define IMUMSPM0_UART_W_TIMEOUT_MS   10U
#define IMUMSPM0_UART_R_TIMEOUT_MS   500U

#define IMUMSPM0_FRAME_WRITE_NUM   5U
#define IMUMSPM0_FRAME_READ_NUM    5U

#define IMUMSPM0_Device_MAXNUM     1U

#define IMUMSPM0_Device_ValidID    0xABCD

static IMUMSPM0_Device Device[IMUMSPM0_Device_MAXNUM] = {0};


void IMUMSPM0_Init(void)
{
   
   BSP_IMUMSPM0_Init(&Device[0]);
}

IMUMSPM0_Device* IMUMSPM0_GetDevice(void)
{

    uint8_t ActiveDeviceNum = 0;
    uint8_t i;
    //软件层实例默认标定0点
    for(i = 0; i < IMUMSPM0_Device_MAXNUM; i++)
    {
        if(Device[i].ID != IMUMSPM0_Device_ValidID)
        {
            Device[i].Data.newDataSpeed = 0;
            Device[i].Data.oldDataSpeed = 0;
            Device[i].Data.newDataAngle = 0;
            Device[i].Data.oldDataAngle = 0;
            Device[i].Data.Angle        = 0.0f;
            Device[i].Data.Speed        = 0.0f;
            Device[i].ErrorCode         = IMUMSPM0_ERR_NONE;
            Device[i].State             = IMUMSPM0_WAIT;
            Device[i].ID                = IMUMSPM0_Device_ValidID;

            break;
        }
        else
        {
            ActiveDeviceNum ++;
            continue;
        }
    }

    if(ActiveDeviceNum != IMUMSPM0_Device_MAXNUM)
    {
        return &Device[i];
    }
    else
    {
        return NULL;
    }
    
}

void IMUMSPM0_SendCommand(IMUMSPM0_Device* Device, uint8_t IMUMSPM0_CMD_IDX,const uint16_t IMUMSPM0_CMD_FUNCTION)
{
    if(Device == NULL)
    {
        return;
    }

    HAL_StatusTypeDef status = HAL_OK;

    uint8_t IMUMSPM0_CMD_Buf[IMUMSPM0_FRAME_WRITE_NUM];
    uint8_t IMUMSPM0_STA_Buf[IMUMSPM0_FRAME_READ_NUM];

    IMUMSPM0_CMD_Buf[0] = IMUMSPM0_FRAME_WRITE_HEAD1;
    IMUMSPM0_CMD_Buf[1] = IMUMSPM0_FRAME_WRITE_HEAD2;



    switch (IMUMSPM0_CMD_IDX)
    {
        case IMUMSPM0_CMD_YAW_ZERO:
            //发送解锁指令
            IMUMSPM0_CMD_Buf[2] = IMUMSPM0_FRAME_KEY_REGADDRESS;
            IMUMSPM0_CMD_Buf[3] = (uint8_t)(IMUMSPM0_FRAME_KEY_FUNCTION_UNLOCK);
            IMUMSPM0_CMD_Buf[4] = (uint8_t)((IMUMSPM0_FRAME_KEY_FUNCTION_UNLOCK) >> 8);
        
            BSP_IMUMSPM0_SendBytes(IMUMSPM0_CMD_Buf, IMUMSPM0_FRAME_WRITE_NUM, IMUMSPM0_UART_W_TIMEOUT_MS);
            DWT_Delay_ms(100);

            //发送Yaw角归0指令
            IMUMSPM0_CMD_Buf[2] = IMUMSPM0_FRAME_YAW_ZERO_REGADDRESS;
            IMUMSPM0_CMD_Buf[3] = (uint8_t)(IMUMSPM0_FRAME_YAW_ZERO_FUNCTION_SET);
            IMUMSPM0_CMD_Buf[4] = (uint8_t)((IMUMSPM0_FRAME_YAW_ZERO_FUNCTION_SET) >> 8);
        
            BSP_IMUMSPM0_SendBytes(IMUMSPM0_CMD_Buf, IMUMSPM0_FRAME_WRITE_NUM, IMUMSPM0_UART_W_TIMEOUT_MS);
            DWT_Delay_ms(100);

            //发送保存数据指令
            IMUMSPM0_CMD_Buf[2] = IMUMSPM0_FRAME_SAVE_REGADDRESS;
            IMUMSPM0_CMD_Buf[3] = (uint8_t)(IMUMSPM0_FRAME_SAVE_FUNCTION_RESTORE);
            IMUMSPM0_CMD_Buf[4] = (uint8_t)((IMUMSPM0_FRAME_SAVE_FUNCTION_RESTORE) >> 8);
        
            BSP_IMUMSPM0_SendBytes(IMUMSPM0_CMD_Buf, IMUMSPM0_FRAME_WRITE_NUM, IMUMSPM0_UART_W_TIMEOUT_MS);
            break;

        case IMUMSPM0_CMD_BIAS_CAL:
            //发送解锁指令
            IMUMSPM0_CMD_Buf[2] = IMUMSPM0_FRAME_KEY_REGADDRESS;
            IMUMSPM0_CMD_Buf[3] = (uint8_t)(IMUMSPM0_FRAME_KEY_FUNCTION_UNLOCK);
            IMUMSPM0_CMD_Buf[4] = (uint8_t)((IMUMSPM0_FRAME_KEY_FUNCTION_UNLOCK) >> 8);
        
            BSP_IMUMSPM0_SendBytes(IMUMSPM0_CMD_Buf, IMUMSPM0_FRAME_WRITE_NUM, IMUMSPM0_UART_W_TIMEOUT_MS);
            DWT_Delay_ms(100);

            //发送自动获取零偏指令
            IMUMSPM0_CMD_Buf[2] = IMUMSPM0_FRAME_BIAS_CAL_REGADDRESS;
            IMUMSPM0_CMD_Buf[3] = (uint8_t)(IMUMSPM0_FRAME_BIAS_CAL_FUNCTION_AUTOBIAS);
            IMUMSPM0_CMD_Buf[4] = (uint8_t)((IMUMSPM0_FRAME_BIAS_CAL_FUNCTION_AUTOBIAS) >> 8);

            BSP_IMUMSPM0_SendBytes(IMUMSPM0_CMD_Buf, IMUMSPM0_FRAME_WRITE_NUM, IMUMSPM0_UART_W_TIMEOUT_MS);
            DWT_Delay(19.5);

            //发送读取状态寄存器值指令
            IMUMSPM0_CMD_Buf[2] = IMUMSPM0_FRAME_BIAS_CAL_STATE_REGADDRESS;
            IMUMSPM0_CMD_Buf[3] = (uint8_t)(IMUMSPM0_FRAME_BIAS_CAL_FUNCTION_GETSTATE);
            IMUMSPM0_CMD_Buf[4] = (uint8_t)((IMUMSPM0_FRAME_BIAS_CAL_FUNCTION_GETSTATE) >> 8);
        
            BSP_IMUMSPM0_SendBytes(IMUMSPM0_CMD_Buf, IMUMSPM0_FRAME_WRITE_NUM, IMUMSPM0_UART_W_TIMEOUT_MS);
            //获取实际状态
            BSP_IMUMSPM0_ReadBytes(IMUMSPM0_STA_Buf, IMUMSPM0_FRAME_READ_NUM, IMUMSPM0_UART_R_TIMEOUT_MS, &status);
            //错误判断机制
            if(status == HAL_TIMEOUT)
            {
                Device ->ErrorCode = IMUMSPM0_ERR_TIMEOUT;
                return;
            }
            else if(status == HAL_OK)
            {
                uint16_t CRC_SUM = 0;
                for(int i = 0; i < IMUMSPM0_FRAME_READ_NUM; i++)
                {
                    CRC_SUM += IMUMSPM0_STA_Buf[i];
                }
                if(CRC_SUM != IMUMSPM0_FRAME_BIAS_CAL_STATE_SUCCESSCODE)
                {
                    Device ->ErrorCode = IMUMSPM0_ERR_STATE;
                    return;
                }
            }
            break;

        case IMUMSPM0_CMD_SCALE_FACTOR:
            //发送解锁指令
            IMUMSPM0_CMD_Buf[2] = IMUMSPM0_FRAME_KEY_REGADDRESS;
            IMUMSPM0_CMD_Buf[3] = (uint8_t)(IMUMSPM0_FRAME_KEY_FUNCTION_UNLOCK);
            IMUMSPM0_CMD_Buf[4] = (uint8_t)((IMUMSPM0_FRAME_KEY_FUNCTION_UNLOCK) >> 8);
        
            BSP_IMUMSPM0_SendBytes(IMUMSPM0_CMD_Buf, IMUMSPM0_FRAME_WRITE_NUM, IMUMSPM0_UART_W_TIMEOUT_MS);
            DWT_Delay_ms(100);

            //发送开始手动标定指令
            IMUMSPM0_CMD_Buf[2] = IMUMSPM0_FRAME_SCALE_FACTOR_REGADDRESS;
            IMUMSPM0_CMD_Buf[3] = (uint8_t)(IMUMSPM0_FRAME_SCALE_FACTOR_FUNCTION_MANU);
            IMUMSPM0_CMD_Buf[4] = (uint8_t)((IMUMSPM0_FRAME_SCALE_FACTOR_FUNCTION_MANU) >> 8);
        
            BSP_IMUMSPM0_SendBytes(IMUMSPM0_CMD_Buf, IMUMSPM0_FRAME_WRITE_NUM, IMUMSPM0_UART_W_TIMEOUT_MS);
            //延时10s用手转一圈
            DWT_Delay(10);

            //发送保存并结束标定指令
            IMUMSPM0_CMD_Buf[2] = IMUMSPM0_FRAME_SAVE_REGADDRESS;
            IMUMSPM0_CMD_Buf[3] = (uint8_t)(IMUMSPM0_FRAME_SAVE_FUNCTION_RESTORE);
            IMUMSPM0_CMD_Buf[4] = (uint8_t)((IMUMSPM0_FRAME_SAVE_FUNCTION_RESTORE) >> 8);
        
            BSP_IMUMSPM0_SendBytes(IMUMSPM0_CMD_Buf, IMUMSPM0_FRAME_WRITE_NUM, IMUMSPM0_UART_W_TIMEOUT_MS);

        case IMUMSPM0_CMD_BUAD:
            //发送解锁指令
            IMUMSPM0_CMD_Buf[2] = IMUMSPM0_FRAME_KEY_REGADDRESS;
            IMUMSPM0_CMD_Buf[3] = (uint8_t)(IMUMSPM0_FRAME_KEY_FUNCTION_UNLOCK);
            IMUMSPM0_CMD_Buf[4] = (uint8_t)((IMUMSPM0_FRAME_KEY_FUNCTION_UNLOCK) >> 8);
        
            BSP_IMUMSPM0_SendBytes(IMUMSPM0_CMD_Buf, IMUMSPM0_FRAME_WRITE_NUM, IMUMSPM0_UART_W_TIMEOUT_MS);
            DWT_Delay_ms(100);

            //发送设置波特率指令
            IMUMSPM0_CMD_Buf[2] = IMUMSPM0_FRAME_BUAD_REGADDRESS;
            IMUMSPM0_CMD_Buf[3] = (uint8_t)(IMUMSPM0_CMD_FUNCTION);
            IMUMSPM0_CMD_Buf[4] = (uint8_t)((IMUMSPM0_CMD_FUNCTION) >> 8);
        
            BSP_IMUMSPM0_SendBytes(IMUMSPM0_CMD_Buf, IMUMSPM0_FRAME_WRITE_NUM, IMUMSPM0_UART_W_TIMEOUT_MS);

            DWT_Delay_ms(100);

            //发送保存数据指令
            IMUMSPM0_CMD_Buf[2] = IMUMSPM0_FRAME_SAVE_REGADDRESS;
            IMUMSPM0_CMD_Buf[3] = (uint8_t)(IMUMSPM0_FRAME_SAVE_FUNCTION_RESTORE);
            IMUMSPM0_CMD_Buf[4] = (uint8_t)((IMUMSPM0_FRAME_SAVE_FUNCTION_RESTORE) >> 8);
        
            BSP_IMUMSPM0_SendBytes(IMUMSPM0_CMD_Buf, IMUMSPM0_FRAME_WRITE_NUM, IMUMSPM0_UART_W_TIMEOUT_MS);

            DWT_Delay_ms(100);

            //主控板同步波特率
            switch (IMUMSPM0_CMD_FUNCTION)
            {
                case IMUMSPM0_FRAME_BUAD_FUNCTION_2400bps:
                    status = BSP_IMUMSPM0_SetBuadrate(2400);
                    break;
                case IMUMSPM0_FRAME_BUAD_FUNCTION_4800bps:
                    status = BSP_IMUMSPM0_SetBuadrate(4800);
                    break;
                case IMUMSPM0_FRAME_BUAD_FUNCTION_9600bps:
                    status = BSP_IMUMSPM0_SetBuadrate(9600);
                    break;
                case IMUMSPM0_FRAME_BUAD_FUNCTION_19200bps:
                    status = BSP_IMUMSPM0_SetBuadrate(19200);
                    break;
                case IMUMSPM0_FRAME_BUAD_FUNCTION_38400bps:
                    status = BSP_IMUMSPM0_SetBuadrate(38400);
                    break;
                case IMUMSPM0_FRAME_BUAD_FUNCTION_57600bps:
                    status = BSP_IMUMSPM0_SetBuadrate(57600);
                    break;
                case IMUMSPM0_FRAME_BUAD_FUNCTION_115200bps:
                    status = BSP_IMUMSPM0_SetBuadrate(115200);
                break;
                case IMUMSPM0_FRAME_BUAD_FUNCTION_230400bps:
                    status = BSP_IMUMSPM0_SetBuadrate(230400);
                    break;
            default:
                    Device ->ErrorCode = IMUMSPM0_ERR_FRAME;
                    return;
            }
            if(status != HAL_OK)
            {
                Device ->ErrorCode = IMUMSPM0_ERR_INIT;
                return;
            }
            
            case IMUMSPM0_CMD_RATE:
            //发送解锁指令
            IMUMSPM0_CMD_Buf[2] = IMUMSPM0_FRAME_KEY_REGADDRESS;
            IMUMSPM0_CMD_Buf[3] = (uint8_t)(IMUMSPM0_FRAME_KEY_FUNCTION_UNLOCK);
            IMUMSPM0_CMD_Buf[4] = (uint8_t)((IMUMSPM0_FRAME_KEY_FUNCTION_UNLOCK) >> 8);
        
            BSP_IMUMSPM0_SendBytes(IMUMSPM0_CMD_Buf, IMUMSPM0_FRAME_WRITE_NUM, IMUMSPM0_UART_W_TIMEOUT_MS);
            DWT_Delay_ms(100);

            //发送设置反馈频率指令
            IMUMSPM0_CMD_Buf[2] = IMUMSPM0_FRAME_RATE_REGADDRESS;
            IMUMSPM0_CMD_Buf[3] = (uint8_t)(IMUMSPM0_CMD_FUNCTION);
            IMUMSPM0_CMD_Buf[4] = (uint8_t)((IMUMSPM0_CMD_FUNCTION) >> 8);
        
            BSP_IMUMSPM0_SendBytes(IMUMSPM0_CMD_Buf, IMUMSPM0_FRAME_WRITE_NUM, IMUMSPM0_UART_W_TIMEOUT_MS);
            DWT_Delay_ms(100);

            //发送保存数据指令
            IMUMSPM0_CMD_Buf[2] = IMUMSPM0_FRAME_SAVE_REGADDRESS;
            IMUMSPM0_CMD_Buf[3] = (uint8_t)(IMUMSPM0_FRAME_SAVE_FUNCTION_RESTORE);
            IMUMSPM0_CMD_Buf[4] = (uint8_t)((IMUMSPM0_FRAME_SAVE_FUNCTION_RESTORE) >> 8);
        
            BSP_IMUMSPM0_SendBytes(IMUMSPM0_CMD_Buf, IMUMSPM0_FRAME_WRITE_NUM, IMUMSPM0_UART_W_TIMEOUT_MS);
            break;

            case IMUMSPM0_CMD_SAVE:
            //发送解锁指令
            IMUMSPM0_CMD_Buf[2] = IMUMSPM0_FRAME_KEY_REGADDRESS;
            IMUMSPM0_CMD_Buf[3] = (uint8_t)(IMUMSPM0_FRAME_KEY_FUNCTION_UNLOCK);
            IMUMSPM0_CMD_Buf[4] = (uint8_t)((IMUMSPM0_FRAME_KEY_FUNCTION_UNLOCK) >> 8);
        
            BSP_IMUMSPM0_SendBytes(IMUMSPM0_CMD_Buf, IMUMSPM0_FRAME_WRITE_NUM, IMUMSPM0_UART_W_TIMEOUT_MS);
            DWT_Delay_ms(100);

            //发送重启或恢复出厂指令
            IMUMSPM0_CMD_Buf[2] = IMUMSPM0_FRAME_SAVE_REGADDRESS;
            
            if(IMUMSPM0_CMD_FUNCTION == IMUMSPM0_FRAME_SAVE_FUNCTION_RESTART)
            {
                IMUMSPM0_CMD_Buf[3] = (uint8_t)(IMUMSPM0_FRAME_SAVE_FUNCTION_RESTART);
                IMUMSPM0_CMD_Buf[4] = (uint8_t)((IMUMSPM0_FRAME_SAVE_FUNCTION_RESTART) >> 8);
                BSP_IMUMSPM0_SendBytes(IMUMSPM0_CMD_Buf, IMUMSPM0_FRAME_WRITE_NUM, IMUMSPM0_UART_W_TIMEOUT_MS);
                DWT_Delay_ms(100);
            }
            
            else if(IMUMSPM0_CMD_FUNCTION == IMUMSPM0_FRAME_SAVE_FUNCTION_DEFAULT)
            {
                IMUMSPM0_CMD_Buf[3] = (uint8_t)(IMUMSPM0_FRAME_SAVE_FUNCTION_DEFAULT);
                IMUMSPM0_CMD_Buf[4] = (uint8_t)((IMUMSPM0_FRAME_SAVE_FUNCTION_DEFAULT) >> 8);
                BSP_IMUMSPM0_SendBytes(IMUMSPM0_CMD_Buf, IMUMSPM0_FRAME_WRITE_NUM, IMUMSPM0_UART_W_TIMEOUT_MS);
                DWT_Delay_ms(100);

                //发送保存数据指令
                IMUMSPM0_CMD_Buf[2] = IMUMSPM0_FRAME_SAVE_REGADDRESS;
                IMUMSPM0_CMD_Buf[3] = (uint8_t)(IMUMSPM0_FRAME_SAVE_FUNCTION_RESTORE);
                IMUMSPM0_CMD_Buf[4] = (uint8_t)((IMUMSPM0_FRAME_SAVE_FUNCTION_RESTORE) >> 8);
        
                BSP_IMUMSPM0_SendBytes(IMUMSPM0_CMD_Buf, IMUMSPM0_FRAME_WRITE_NUM, IMUMSPM0_UART_W_TIMEOUT_MS);
            }
            
            

        default:
            Device ->ErrorCode = IMUMSPM0_ERR_FRAME;
            return;
    }
    Device ->ErrorCode = IMUMSPM0_ERR_NONE;

}

/*
void IMUMSPM0_GetDataCallback(IMUMSPM0_Device* Device)
{
     uint8_t DataByte = 0;
    static uint16_t CRC_SUM = 0;

    HAL_StatusTypeDef status = HAL_OK;

    IMUMSPM0_RecieveCheck(Device);

    if(Device ->State == IMUMSPM0_WAIT)
    {
        BSP_IMUMSPM0_ReadBytes_IT(&DataByte, 1, &status);
        if(DataByte == IMUMSPM0_FRAME_READ_HEAD)
        {
            CRC_SUM += DataByte;
            Device ->State = IMUMSPM0_TYPE;
        }
    }
    else if(Device ->State == IMUMSPM0_TYPE)
    {
        BSP_IMUMSPM0_ReadBytes_IT(&DataByte, 1, &status);
        if(DataByte == IMUMSPM0_FRAME_TYPE_SPEED || DataByte == IMUMSPM0_FRAME_TYPE_ANGLE || DataByte == IMUMSPM0_FRAME_TYPE_STATE)
        {
            CRC_SUM += DataByte;
            if(DataByte == IMUMSPM0_FRAME_TYPE_SPEED)
            {
                Device ->Data.DataType     = IMUMSPM0_TYPE_SPEED;
                Device ->Data.oldDataSpeed = Device ->Data.newDataSpeed;
                Device ->Data.newDataSpeed = 0U;
                
            }
            else if(DataByte == IMUMSPM0_FRAME_TYPE_ANGLE)
            {
                Device ->Data.DataType     = IMUMSPM0_TYPE_ANGLE;
                Device ->Data.oldDataAngle = Device ->Data.newDataAngle;
                Device ->Data.newDataAngle = 0U;
                
            }
            Device ->State = IMUMSPM0_DATAL;
        }
        else
        {
            Device ->State = IMUMSPM0_WAIT;
        }
    }
    else if(Device ->State == IMUMSPM0_DATAL)
    {
        BSP_IMUMSPM0_ReadBytes_IT(&DataByte, 1, &status);
        CRC_SUM += DataByte;
        if(Device ->Data.DataType == IMUMSPM0_TYPE_SPEED)
        {
            Device ->Data.newDataSpeed |= DataByte;
        }
        else if(Device ->Data.DataType == IMUMSPM0_TYPE_ANGLE)
        {
            Device ->Data.newDataAngle |= DataByte;
        }
        Device ->State = IMUMSPM0_DATAH;
    }
    else if(Device ->State == IMUMSPM0_DATAH)
    {
        BSP_IMUMSPM0_ReadBytes_IT(&DataByte, 1, &status);

        CRC_SUM += DataByte;

        if(Device ->Data.DataType == IMUMSPM0_TYPE_SPEED)
        {
            Device ->Data.newDataSpeed  |= (short)((short)DataByte << 8);
        }
        else if(Device ->Data.DataType == IMUMSPM0_TYPE_ANGLE)
        {
             Device ->Data.newDataAngle |= (short)((short)DataByte << 8);
        }
       
        Device ->State = IMUMSPM0_MCRC;
    }
    else if(Device ->State == IMUMSPM0_MCRC)
    {
        BSP_IMUMSPM0_ReadBytes_IT(&DataByte, 1, &status);

        if((CRC_SUM & 0xFF) != DataByte)
        {
            Device ->ErrorCode = IMUMSPM0_ERR_CRC;
        }
        Device ->State = IMUMSPM0_WAIT;
        //重置CRC
        CRC_SUM = 0;
    }
    else
    {
		
        Device ->State = IMUMSPM0_WAIT;
    }
}
*/

void IMUMSPM0_GetDataCallback(IMUMSPM0_Device* Device)
{

    HAL_StatusTypeDef status = HAL_OK;
    static uint16_t CRC_SUM = 0;

   
    switch(Device->State)
    {
        case IMUMSPM0_WAIT:
            if(Device ->Data.DataByte == IMUMSPM0_FRAME_READ_HEAD)
            {
                CRC_SUM = Device ->Data.DataByte;
                Device->State = IMUMSPM0_TYPE;
            }
            break;

        case IMUMSPM0_TYPE:
            if(Device ->Data.DataByte == IMUMSPM0_FRAME_TYPE_SPEED || 
               Device ->Data.DataByte == IMUMSPM0_FRAME_TYPE_ANGLE || 
               Device ->Data.DataByte == IMUMSPM0_FRAME_TYPE_STATE)
            {
                CRC_SUM += Device ->Data.DataByte;
                Device->Data.DataType = (Device ->Data.DataByte == IMUMSPM0_FRAME_TYPE_SPEED) ? IMUMSPM0_TYPE_SPEED : IMUMSPM0_TYPE_ANGLE;
                // 注意：这里不要动 oldData！校验通过后再更新
                Device->Data.newDataSpeed = 0;
                Device->Data.newDataAngle = 0;
                Device->State = IMUMSPM0_DATAL;
            }
            else
            {
                // 类型不对，复位等待帧头
                Device->State = IMUMSPM0_WAIT;
                CRC_SUM = 0;
            }
            break;

        case IMUMSPM0_DATAL:
            CRC_SUM += Device ->Data.DataByte;
            if(Device->Data.DataType == IMUMSPM0_TYPE_SPEED)
                Device->Data.newDataSpeed = (int16_t)Device ->Data.DataByte;
            else
                Device->Data.newDataAngle = (int16_t)Device ->Data.DataByte;
            Device->State = IMUMSPM0_DATAH;
            break;

        case IMUMSPM0_DATAH:
            CRC_SUM += Device ->Data.DataByte;
            if(Device->Data.DataType == IMUMSPM0_TYPE_SPEED)
                Device->Data.newDataSpeed |= (int16_t)((int16_t)Device ->Data.DataByte << 8);
            else
                Device->Data.newDataAngle |= (int16_t)((int16_t)Device ->Data.DataByte << 8);
            Device->State = IMUMSPM0_MCRC;
            break;

        case IMUMSPM0_MCRC:
            if((CRC_SUM & 0xFF) == Device ->Data.DataByte)
            {
                // 校验通过：在这里才把新数据更新到 oldData
                Device->ErrorCode = IMUMSPM0_ERR_NONE;
                if(Device->Data.DataType == IMUMSPM0_TYPE_SPEED)
                    Device->Data.oldDataSpeed = Device->Data.newDataSpeed;
                else
                    Device->Data.oldDataAngle = Device->Data.newDataAngle;
            }
            else
            {
                Device->ErrorCode = IMUMSPM0_ERR_CRC;
            }
            // 一帧结束，无论成败都复位
            Device->State = IMUMSPM0_WAIT;
            CRC_SUM = 0;
            break;

        default:
            Device->State = IMUMSPM0_WAIT;
            CRC_SUM = 0;
            break;
    }
    // 立刻开启下一个字节的接收
    BSP_IMUMSPM0_ReadBytes_IT((uint8_t*)&(Device->Data.DataByte), 1, &status);
}


void IMUMSPM0_GetData(IMUMSPM0_Device* Device)
{
    if(Device == NULL)
    {
        return;
    }
    //数据校验正确
    if(Device ->ErrorCode != IMUMSPM0_ERR_CRC && Device ->ErrorCode != IMUMSPM0_ERR_FRAME)
    {
        Device ->Data.Speed = (float)(Device ->Data.oldDataSpeed)*2000.0f/32768.0f;
        Device ->Data.Angle = (float)(Device ->Data.oldDataAngle)*180.0f/32768.0f;
    }
	//错误数据丢弃
    else
    {
		Device ->ErrorCode = IMUMSPM0_ERR_NONE;
        return;
    }
}

void IMUMSPM0_RecieveCheck(IMUMSPM0_Device* Device)
{
    static uint8_t error_time = 0;
    static IMUMSPM0_State old_state = IMUMSPM0_WAIT;
    static IMUMSPM0_State new_state = IMUMSPM0_WAIT;

    if(Device == NULL)
    {
        return;
    }
    //更新状态值
    new_state = Device ->State;
    //如果上次的状态切换失败，增加错误次数
    if(new_state == old_state)
    {
        error_time++;
    }
    //如果状态切换成功就更新为当前状态，并清零错误次数，默认该帧完好
    else
    {
        error_time = 0;
        old_state = new_state;
    }
    //如果次数达到三次以上即认为出现不可挽回错误丢弃该帧，切换至重新等待帧头状态
    if(error_time >= 3)
    {
        error_time = 0;
        
        Device ->ErrorCode = IMUMSPM0_ERR_FRAME;
        Device ->State = IMUMSPM0_WAIT;
    }

}



// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
//     if(huart == IMUMSPM0_UART_HANDLE)
//     {

       
//         // 进来的时候 g_imu_rx_byte 里已经是有效的1个字节了

//        //目前只有一个陀螺仪先写死
//         IMUMSPM0_GetDataCallback(&Device[0]);
        
//         // 立刻开启下一个字节的接收
//      //   BSP_IMUMSPM0_ReadBytes_IT((uint8_t*)&(Device[0].Data.DataByte), 1, &status);
       
//     }
// }


