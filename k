compressor.c:79:    self->bfr_l = (float*)malloc(256*sizeof(float));
compressor.c:80:    self->bfr_r = (float*)malloc(256*sizeof(float));
compressor.c:82:    compressor_init(&self->compressor_state, samplerate);
compressor.c:94:            self->input_left = (float*) data;
compressor.c:97:            self->input_right = (float*) data;
compressor.c:101:            self->output_left = (float*) data;
compressor.c:104:            self->output_right = (float*) data;
compressor.c:108:            self->threshold = (float*) data;
compressor.c:111:            self->knee = (float*) data;
compressor.c:114:            self->attack = (float*) data;
compressor.c:117:            self->release = (float*) data;
compressor.c:120:            self->ratio = (float*) data;
compressor.c:123:            self->makeup = (float*) data;
compressor.c:127:            self->volume = (float*) data;
compressor.c:142:    if ((self->prev_threshold != (float)*self->threshold) || (self->prev_knee != (float)*self->knee) || (self->prev_attack != (float)*self->attack) || (self->prev_release != (float)*self->release) || (self->prev_ratio != (float)*self->ratio) || (self->prev_makeup != (float)*self->makeup) )
compressor.c:144:        compressor_set_params(&self->compressor_state, (float)*self->threshold,
compressor.c:145:                                (float)*self->knee, (float)*self->ratio, ((float)*self->attack / 1000), ((float)*self->release / 1000), (float)*self->makeup);
compressor.c:147:        self->prev_threshold = (float)*self->threshold;
compressor.c:148:        self->prev_knee = (float)*self->knee;
compressor.c:149:        self->prev_attack = (float)*self->attack;
compressor.c:150:        self->prev_release = (float)*self->release;
compressor.c:151:        self->prev_ratio = (float)*self->ratio;
compressor.c:152:        self->prev_makeup = (float)*self->makeup;
compressor.c:155:    compressor_process(&self->compressor_state, n_samples, self->input_left, self->input_left, self->bfr_l, self->bfr_r);
compressor.c:159:        self->output_left[i] = self->bfr_l[i] * cmop_db2lin((float)*self->volume);
compressor.c:160:        self->output_right[i] = self->bfr_r[i] * cmop_db2lin((float)*self->volume);
