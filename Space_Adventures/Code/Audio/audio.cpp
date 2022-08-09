namespace Audio {
	global_variable float global_volume, sound_volume, music_volume, UI_volume;

	enum sound_type {
		ST_buy,
		ST_failed_to_buy,

		ST_blaster1,
		ST_blaster2,
		
		ST_star_desctroyer,

		ST_COUNT,
	};
	enum UI_sound_type {
		UI_focus,

		UI_COUNT,
	};
	enum music_type {
		MT_menu,
		MT_gameplay,
		MT_gameover,
		MT_boss,
		MT_shop,

		MT_COUNT
	};

	enum music_condition {
		SC_stop, // не играет
		SC_down, // ирает(постепенно уменьшаем громкость)
		SC_up,   // ирает(постепенно увеличиваем громкость)
		SC_play, // играет
	};

	struct sound {
		OutputStreamPtr* sound_ptr;
		size_t size;

		void play() {
			for (size_t i = 0; i < size; i++) {
				if (!sound_ptr[i]->isPlaying()) {
					sound_ptr[i]->setVolume(sound_volume * global_volume);
					sound_ptr[i]->play();
					break;
				}
			}
		}
		void stop() {
			for (size_t i = 0; i < size; i++) {
				sound_ptr[i]->stop();
			}
		}

		/*void repeat() {
			for (int i = 0; i < size; i++) {
				sound_ptr[i]->setRepeat(true);
			}
		}*/
		/*void no_repeat() {
			for (int i = 0; i < size; i++) {
				sound_ptr[i]->setRepeat(false);
			}
		}*/

		bool is_play() {
			size_t i;
			for (i = 0; i < size && sound_ptr[i]->isPlaying() == false; i++) {
			}
			return i < size;
		}

		void OpenSound(AudioDevicePtr& device, const char* path, bool streaming, size_t _size) {
			size = _size;
			sound_ptr = new OutputStreamPtr[_size];
			for (size_t i = 0; i < _size; i++) {
				sound_ptr[i] = audiere::OpenSound(device, path, streaming);
			}
		}
	};
	struct UI_sound {
		OutputStreamPtr* sound_ptr;
		size_t size;

		inline void play() {
			for (size_t i = 0; i < size; i++) {
				if (!sound_ptr[i]->isPlaying()) {
					sound_ptr[i]->setVolume(UI_volume * global_volume);
					sound_ptr[i]->play();
					break;
				}
			}
		}
		void OpenSound(AudioDevicePtr& device, const char* path, bool streaming, size_t _size) {
			size = _size;
			sound_ptr = new OutputStreamPtr[_size];
			for (size_t i = 0; i < _size; i++) {
				sound_ptr[i] = audiere::OpenSound(device, path, streaming);
			}
		}
	};
	struct music {
		OutputStreamPtr sound_ptr;
		music_condition sound_cond;

		void update(float dt) {
			if (sound_cond == SC_down) {
				float Volume = max(0, sound_ptr->getVolume() - dt * .5f);
				sound_ptr->setVolume(Volume);
				if (Volume == 0) {
					sound_cond = SC_stop;
					sound_ptr->stop();
				}
			}
			else if (sound_cond == SC_up) {
				float Volume = min(music_volume, sound_ptr->getVolume() + dt * .3f);
				sound_ptr->setVolume(Volume);
				if (Volume == music_volume) {
					sound_cond = SC_play;
				}
			}
		}
	};

	sound Sounds[ST_COUNT];
	music Musics[MT_COUNT];
	UI_sound UI[UI_COUNT];

	void init_musics(AudioDevicePtr& device, std::list<std::string> Name) {
		size_t index = 0;
		for (auto it_name = Name.begin(); it_name != Name.end(); it_name++) {
			Musics[index].sound_ptr = OpenSound(device, ("Audio\\Musics\\" + *it_name).c_str(), true);
			index++;
		}
	}
	void init_sounds(AudioDevicePtr& device, std::list<std::string> Name) {
		size_t index = 0;
		for (auto it_name = Name.begin(); it_name != Name.end(); it_name++) {
			Sounds[index].OpenSound(device, ("Audio\\Sounds\\" + *it_name).c_str(), true, 15);
			index++;
		}
	}
	void init_UI(AudioDevicePtr& device, std::list<std::string> Name) {
		size_t index = 0;
		for (auto it_name = Name.begin(); it_name != Name.end(); it_name++) {
			UI[index].OpenSound(device, ("Audio\\Sounds\\" + *it_name).c_str(), true, 15);
			index++;
		}
	}

	void init_audio(AudioDevicePtr& device) {
		init_musics(device, { "Snowing_Outside.mp3", "Skycrapers.mp3", "Sadness.mp3", "Dramatic_Battle.mp3", "Item_Shop.mp3" });
		init_sounds(device, { "buy.mp3", "failed to buy.mp3", "blaster1.mp3", "blaster2.mp3", "star destroyer.mp3" });
		init_UI(device, { "UI.mp3" });
	}

	void play_music(music_type mus) {
		Musics[mus].sound_cond = SC_up;
		Musics[mus].sound_ptr->setVolume(0);
		Musics[mus].sound_ptr->play();
		Musics[mus].sound_ptr->setRepeat(true);
	}
	void stop_music(music_type mus) {
		Musics[mus].sound_cond = SC_down;
	}

	void change_of_music(music_type _old, music_type _new) {
		stop_music(_old);
		play_music(_new);
	}

	void update_musics(float dt) {
		for (int i = 0; i < MT_COUNT; i++) {
			Musics[i].update(dt);
		}
	}

	void update_musics_volume() {
		for (int i = 0; i < MT_COUNT; i++) {
			Musics[i].sound_ptr->setVolume(music_volume * global_volume);
		}
	}
}

using namespace Audio;