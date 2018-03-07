#include "base.h"
#include "exportfuncs.h"
#include "client.h"
#include "configs.h"
#include "imagelib.h"
#include "texdraw.h"
#include "util.h"

#include "muzzleflash.h"

CMuzzleflashManager gMuzzleflashManager;
CMuzzleflashReplaceManager gMuzzleflashReplaceManager;

void CMuzzleflashManager::Init()
{
	if (!gConfigs.bEnableNewMuzzle)
		return;

	gMuzzleflashReplaceManager.ReadData();

	int total = 0, j = 0;
	start[total] = 0;

	char configPath[MAX_PATH], line[128];
	FILE *fp;

	sprintf(configPath, "cstrike\\sprites\\muzzleflash\\muzzleflash.txt");
	fp = fopen(configPath, "r");

	if (fp)
	{
		while (!feof(fp))
		{
			fgets(line, sizeof(line), fp);

			if (!line[0] || line[0] == ';' || line[0] == '\n' || line[0] == '\r')
				continue;

			char *p;
			p = strtok(line, " ");

			if (p)
			{
				char filename[128];
				strcpy(filename, p);
				p = strtok(NULL, " ");
				max_frame[total] = atoi(p);
				p = strtok(NULL, " ");
				frame_interval[total] = atof(p);
				p = strtok(NULL, " ");
				follow[total] = (p[0] == '1');
				p = strtok(NULL, " ");
				scalex[total] = atof(p);

				for (int i = 0; i < max_frame[total]; i++)
				{
					char path[_MAX_PATH];
					sprintf(path, "cstrike\\sprites\\muzzleflash\\%s-%d.tga", filename, i + 1);
					LoadTexture(path, id[j], NULL);
					j++;
				}				
				total++;
				start[total] = j;
			}
		}

		fclose(fp);
	}
	else
	{
		LogToFile("Muzzleflash config reading failed. Disabled.");
		gConfigs.bEnableNewMuzzle = false;
	}
}

void CMuzzleflashManager::Add(int id, float curtime, int attachment, float size, float angle)
{
	CMuzzleflash *add = new CMuzzleflash(this->id + start[id], max_frame[id], frame_interval[id], curtime, attachment, size * scalex[id], angle, follow[id]);
	
	if (head == NULL)
	{
		head = add;
	}
	else
	{
		CMuzzleflash *temp = head;

		while (temp->next != NULL)
			temp = temp->next;

		temp->next = add;
	}
}

void CMuzzleflashManager::Draw(float time)
{
	if (!gConfigs.bEnableNewMuzzle)
		return;
	
	Vector right, up;
	gEngfuncs.pfnAngleVectors(v_angles, NULL, right, up);
	Vector attachment[4];
	for (int i = 0; i < 4; i++)
		attachment[i] = gEngfuncs.GetViewModel()->attachment[i];
	
	CMuzzleflash *m = head;
	CMuzzleflash *last;

	while (m)
	{
		m->Draw(time, right, up, attachment);

		if (time > m->die)
		{
			CMuzzleflash *temp = m;

			if (m == head)
			{
				head = m->next;
				m = head;
			}
			else
			{				
				m = m->next;
				last->next = m;
			}
			delete temp;
		}
		else
		{
			last = m;
			m = m->next;
		}
	}
}

void CMuzzleflash::Calc(float time)
{
	frame = max_frame - (die - time) / frame_interval;
	if (frame >= max_frame)
		frame = max_frame - 1;

	//gEngfuncs.Con_DPrintf("%d\n", frame);
}


void CMuzzleflash::Draw(float time, Vector right, Vector up, Vector attach[4])
{	
	Calc(time);

	if (!follow && !origin_saved)
	{
		this->origin = attach[attachment];
		this->origin_saved = true;
	}

	float sinf, cosf;

	sinf = sin(0.0174444444444444 * degree) * size;
	cosf = cos(0.0174444444444444 * degree) * size;	

	Vector origin[4];
	Vector center = attach[attachment];

	if (!follow)
		center = this->origin;
	
	//TEXD_SetRenderMode(kRenderTransColor);
	TEXD_SetRenderMode(kRenderTransAdd);
	glDisable(GL_DEPTH_TEST);

	glColor4f(1, 1, 1, 1);

	glBindTexture(GL_TEXTURE_2D, id[frame]);
	glBegin(GL_QUADS);
	

	if (cl_righthand->value)
	{
		origin[0] = center - right * sinf + up * cosf;
		origin[1] = center + right * cosf + up * sinf;
		origin[2] = center + right * sinf - up * cosf;
		origin[3] = center - right * cosf - up * sinf;

		glTexCoord2f(0.0f, 0.0f);	glVertex3fv(origin[0]);
		glTexCoord2f(1.0f, 0.0f);	glVertex3fv(origin[1]);
		glTexCoord2f(1.0f, 1.0f);	glVertex3fv(origin[2]);
		glTexCoord2f(0.0f, 1.0f);	glVertex3fv(origin[3]);
	}
	else
	{
		origin[0] = center + right * sinf + up * cosf;
		origin[1] = center - right * cosf + up * sinf;
		origin[2] = center - right * sinf - up * cosf;
		origin[3] = center + right * cosf - up * sinf;


		glTexCoord2f(0.0f, 0.0f);	glVertex3fv(origin[0]);
		glTexCoord2f(0.0f, 1.0f);	glVertex3fv(origin[3]);
		glTexCoord2f(1.0f, 1.0f);	glVertex3fv(origin[2]);
		glTexCoord2f(1.0f, 0.0f);	glVertex3fv(origin[1]);
	}

	glEnd();

	glEnable(GL_DEPTH_TEST);
	TEXD_SetRenderMode(kRenderNormal);
}

void CMuzzleflashReplaceManager::ReadData()
{
	char configPath[MAX_PATH], line[128];
	FILE *fp;

	sprintf(configPath, "cstrike\\sprites\\muzzleflash\\event_replace.txt");
	fp = fopen(configPath, "r");

	total = 0;

	if (fp)
	{
		while (!feof(fp))
		{
			fgets(line, sizeof(line), fp);

			if (!line[0] || line[0] == ';' || line[0] == '\n' || line[0] == '\r')
				continue;

			char *p;
			p = strtok(line, " ");

			if (p)
			{
				strcpy(name[total], p);
				p = strtok(NULL, " ");
				old_id[total] = atoi(p);
				p = strtok(NULL, " ");
				attach[total] = atoi(p);
				p = strtok(NULL, " ");
				new_id[total] = atoi(p);
				p = strtok(NULL, " ");
				size[total] = atof(p);
				p = strtok(NULL, " ");
				angle[total] = atof(p);
				p = strtok(NULL, " ");
				anim[total] = atoi(p);

				if (angle[total] == -1)
					angle[total] = 45;

				total++;
			}
		}

		fclose(fp);
	}
}

int CMuzzleflashReplaceManager::Add(char model_name[], int muzzle_id, int attachment, float scale)
{
#if _DEBUG
	ReadData();
#endif

	for (int i = 0; i < total; i++)
	{
		if (!strcmp(model_name, name[i]) && (attach[i] == attachment || attach[i] == -1) && (anim[i] == g_iWeaponAnim || anim[i] == -1))
		{
			float s = size[i];
			if (size[i] == -1)
				s = scale;

			gMuzzleflashManager.Add(new_id[i], gEngfuncs.GetAbsoluteTime(), attachment, s, angle[i]);
			return 1;
		}
	}
	return 0;
}