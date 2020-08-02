#include "mobi.h"
#include <cstdio>

int main(int argc, char** argv) {
    /* Initialize main MOBIData structure */
    /* Must be deallocated with mobi_free() when not needed */
    MOBIData *m = mobi_init();
    if (m == NULL) {
        return -1;
    }

    // char fullpath[100] = "/home/season/libmobi/tests/samples/ncx.mobi";

    /* Open file for reading */
    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        mobi_free(m);
        return -1;
    }

    /* Load file into MOBIData structure */
    /* This structure will hold raw data/metadata from mobi document */
    MOBI_RET mobi_ret = mobi_load_file(m, file);
    fclose(file);
    if (mobi_ret != MOBI_SUCCESS) {
        mobi_free(m);
        return -1;
    }

    /* Initialize MOBIRawml structure */
    /* Must be deallocated with mobi_free_rawml() when not needed */
    /* In the next step this structure will be filled with parsed data */
    MOBIRawml *rawml = mobi_init_rawml(m);
    if (rawml == NULL) {
        mobi_free(m);
        return -1;
    }
    /* Raw data from MOBIData will be converted to html, css, fonts, media
     * resources */
    /* Parsed data will be available in MOBIRawml structure */
    mobi_ret = mobi_parse_rawml(rawml, m);
    if (mobi_ret != MOBI_SUCCESS) {
        mobi_free(m);
        mobi_free_rawml(rawml);
        return -1;
    }

    /* Do something useful here */
    /* ... */
    /* For examples how to access data in MOBIRawml structure see mobitool.c */
    char partname[100];
    printf("=================rawml->markup==================\n");
    if (rawml->markup != NULL) {
        /* Linked list of MOBIPart structures in rawml->markup holds main text
         * files */
        MOBIPart *curr = rawml->markup;
        curr = curr->next;
        // while (curr != NULL) {
        MOBIFileMeta file_meta = mobi_get_filemeta_by_type(curr->type);
        snprintf(partname, sizeof(partname), "OEBPS/part%05zu.%s", curr->uid,
                 file_meta.extension);
        printf("!!!the data size is %d\n", curr->size);
        for (int i = 0; i < curr->size; i++) {
            printf("%c", curr->data[i]);
        }
        printf("\n");
        // curr = curr->next;
        // }
    }

    printf("=================rawml->flow===================\n");
    if (rawml->flow != NULL) {
        /* Linked list of MOBIPart structures in rawml->flow holds supplementary
         * text files */
        MOBIPart *curr = rawml->flow;
        /* skip raw html file */
        curr = curr->next;
        while (curr != NULL) {
            MOBIFileMeta file_meta = mobi_get_filemeta_by_type(curr->type);
            snprintf(partname, sizeof(partname), "OEBPS/flow%05zu.%s",
                     curr->uid, file_meta.extension);
            for (int i = 0; i < curr->size; i++) {
                printf("%c", curr->data[i]);
            }

            printf("\n");

            curr = curr->next;
        }
    }
    // need to define USE_XMLWRITER to parse ncx
    printf("=================resources===================\n");
    // test opf and ncx
    if (rawml->resources != NULL) {
        /* Linked list of MOBIPart structures in rawml->resources holds
        binary
         * files, also opf files */
        MOBIPart *curr = rawml->resources;
        /* jpg, gif, png, bmp, font, audio, video, also opf, ncx */
        while (curr != NULL) {
            MOBIFileMeta file_meta = mobi_get_filemeta_by_type(curr->type);
            if (curr->size > 0) {
                if (file_meta.type == T_OPF||file_meta.type == T_NCX) {
                   for (int i = 0; i < curr->size; i++) {
                        printf("%c", curr->data[i]);
                    }
                    printf("\n");
                }
            }
            curr = curr->next;
        }
    }

    /* Free MOBIRawml structure */
    mobi_free_rawml(rawml);

    /* Free MOBIData structure */
    mobi_free(m);

    return 0;
}