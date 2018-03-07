class CMuzzleflash
{
public:
	CMuzzleflash(int id[], int max_frame, float frame_interval, float curtime, int attachment, float size, float degree = 45, bool follow = true)
	{
		this->die = curtime + max_frame * frame_interval;
		this->next = NULL;
		for (int i = 0; i < max_frame; i++)
			this->id[i] = id[i];
		this->frame = 0;
		this->max_frame = max_frame;
		this->degree = degree;
		this->frame_interval = frame_interval;
		this->attachment = attachment;
		this->size = size;
		this->follow = follow;
		this->origin_saved = false;
	}
	
	void Draw(float time, Vector right, Vector up, Vector attachment[4]);
	void Calc(float time);
	CMuzzleflash *next;
	int id[16];
	float die;
	bool follow;
private:
	int attachment;
	int frame;
	int max_frame;
	float degree;
	float frame_interval;
	float size;

	Vector origin;
	bool origin_saved;
};

class CMuzzleflashManager
{
public:
	CMuzzleflashManager()
	{
		this->head = NULL;
	}

	~CMuzzleflashManager()
	{
		while (this->head != NULL)
		{
			CMuzzleflash *temp = head->next;
			delete head;
			head = temp;
		}
	}

	void Add(int id, float curtime, int attachment, float size, float angle = 45.0f);
	void Draw(float time);
	void Init();
	void VidInit();
public:

private:
	int id[128];
	int start[128];
	int max_frame[128];
	float frame_interval[128];
	bool follow[128];
	float scalex[128];

	CMuzzleflash *head;
};

#define MAX_REPLACE 256

class CMuzzleflashReplaceManager
{
public:
	void ReadData();
	int Add(char name[], int muzzle_id, int attachment, float scale);
private:
	int total;
	char name[MAX_REPLACE][32];
	int old_id[MAX_REPLACE];
	int new_id[MAX_REPLACE];
	float size[MAX_REPLACE];
	float angle[MAX_REPLACE];
	int attach[MAX_REPLACE];
	int anim[MAX_REPLACE];
};
extern CMuzzleflashReplaceManager gMuzzleflashReplaceManager;
extern CMuzzleflashManager gMuzzleflashManager;