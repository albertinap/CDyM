
typedef struct
{
	uint8_t hour;
	uint8_t min;
	uint8_t sec;

}Time;


void RTC_get_time(Time *t);

void RTC_set_time(Time *t);
