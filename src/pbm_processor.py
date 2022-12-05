from cv2 import imread, imwrite, IMREAD_UNCHANGED
from os import listdir
from numpy import concatenate


def load_images(filepath, prefix='cell_', extension='pbm'):
    image_map = dict()

    for filename in listdir(filepath):
        if filename.startswith(prefix) and filename.endswith(extension):
            _, x_coord, y_coord = filename.split('.')[0].split('_')

            x_coord = int(x_coord)
            y_coord = int(y_coord)

            image = imread(filename, IMREAD_UNCHANGED)

            if x_coord in image_map:
                image_map[x_coord].append((y_coord, image))
            else:
                image_map[x_coord] = [(y_coord, image)]

    return {k: sorted(v, key=lambda x: x[0]) for k, v in image_map.items()}


def concat(img_map):
    rows = dict()

    for k, v in img_map.items():
        rows[k] = concatenate([img for _, img in v], axis=1)

    return concatenate([rows[k] for k in sorted(rows.keys())], axis=0)


if __name__ == '__main__':
    imwrite('cell.pbm', concat(load_images('.')))
