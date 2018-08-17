#ifndef timeRing_H
#define timeRing_H

extern  struct ringControl yearRingControl   ;
extern  struct ringControl monthsRingControl ;
extern  struct ringControl dayRingControl    ;
extern  struct ringControl hourRingControl   ;
extern  struct ringControl minRingControl    ;


struct timeControl{
	int controlValue;
	int year;
	int month;
	int day;																	   
	int hour;
	int min;
	
} ;


// Describes a ring control (such as a month table or hour table)
struct ringControl{
	int    control;           // the ring control id (int) in the panel
	int    numCol;		      // number of columns
	int    numRow;			  // number of rows
	int    maxIndex;		  // highest index
	int    origin;		      // index origin (0 or 1)
	size_t offset;            // the offset in the time (tm) struct to the field (month, hour, ...) this control corresponds to
	int (*indexFunc)(int, int, const struct ringControl*);	 // This function calculates the index (in the control) from row/col
};


struct fieldAssoc{
	int    fieldID;				   // the (field) control ID  (e.g. PANEL_assayTimeMonth)
	const struct timeControl* tc;		   // the timeControl associated with that field
	const struct ringControl* rc;		   // the ringControl Associated with that field
};

void displayTime(const struct timeControl *tc);
void initializeTimeField(int fieldID, const struct timeControl* tc, const struct ringControl* rc);
void initializeTimeRings(int ph, int yy, int mm, int dd, int hh, int min, int (*f) (void) );

void hideAllRings(void);

#endif
