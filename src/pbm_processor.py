from PIL import Image
from os import listdir


def load_images(filepath, prefix='cell', extension='pbm'):
    image_map = dict()

    for img in os.listdir(filepath):
        if img.startswith(prefix) and img.endswith(extension):
            _, x_coord, y_coord = img.split('.')[0].split('_')

            x_coord = int(x_coord)
            y_coord = int(y_coord)
            
            cimg = cv2.imread(img, cv2.IMREAD_UNCHANGED)

            if x_coord in image_map:
                image_map[x_coord].append((y_coord, cimg))
            else:
                image_map[x_coord] = [(y_coord, cimg)]
        
    return {k: sorted(v, key=lambda x: x[0]) for k, v in image_map.items()}
        

def concat(img_map):
    rows = dict()
    
    for k, v in img_map.items():
        rows[k] = np.concatenate([img for _, img in v], axis=1)

    return np.concatenate([rows[k] for k in sorted(rows.keys())], axis=0)