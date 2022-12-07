from cv2 import imread, imwrite, IMREAD_UNCHANGED
from os import listdir
from numpy import concatenate


def load_images(filepath, prefix='cell_', extension='pbm'):
    """
    Loads images from filepath.

    :param filepath:    Filepath containing image partitions.
    :param prefix:      File prefix.
    :param extension:   File extension.
    :return:            Dictionary of rows and columns representing entire image.
    """
    image_map = dict()

    # Loop over files in filepath.
    for filename in listdir(filepath):
        # Look only for matching files based on prefix and extension.
        if filename.startswith(prefix) and filename.endswith(extension):
            _, x_coord, y_coord = filename.split('.')[0].split('_')

            x_coord = int(x_coord)
            y_coord = int(y_coord)

            # Read image file.
            image = imread(filename, IMREAD_UNCHANGED)

            if x_coord in image_map:
                image_map[x_coord].append((y_coord, image))
            else:
                image_map[x_coord] = [(y_coord, image)]

    # Sort rows before returning image map.
    return {k: sorted(v, key=lambda x: x[0]) for k, v in image_map.items()}


def concat(img_map):
    """
    Concatenate rows and columns.

    :param img_map: Dictionary of rows and columns produced by load_images.
    :return:        Assembled PBM file.
    """
    rows = dict()

    # Concatenate columns.
    for k, v in img_map.items():
        rows[k] = concatenate([img for _, img in v], axis=1)

    # Concatenate rows.
    return concatenate([rows[k] for k in sorted(rows.keys())], axis=0)


if __name__ == '__main__':
    imwrite('cell.pbm', concat(load_images('.')))
