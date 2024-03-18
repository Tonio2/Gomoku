import pandas as pd
from scipy.stats import pearsonr

# Simulons des données pour l'exemple
# Dans la pratique, vous remplacerez cela par vos données réelles
data = {
    'temps_execution': [145.294, 28.4775, 89.6219, 10.1637, 13.5176, 7.58864, 36.3943, 9.22484, 438.157, 9.45003, 130.468, 45.9909, 45.2173, 58.0574, 532.011, 307.915, 449.747 ],
    'nb_coups_total': [95305, 19694, 66838, 11798, 9269, 8647, 22329, 5134, 259383, 11903, 57761, 31514, 24155, 35101, 361692, 173016, 243867 ],
    'nb_coups_evalues': [72282, 12745, 30085, 3020, 3558, 2339, 16181, 3342, 98436, 3307, 32262, 12628, 12684, 18637, 174349, 74646, 152338 ],
    'nb_coups_necessaires': [71940, 10974, 17403, 2359, 971, 1868, 14105, 2358, 85774, 1795, 30048, 8697, 9387, 14569, 143983, 54293, 138905 ],
    'ratio_evalues': [75, 64, 45, 25, 38, 27, 72, 65, 37, 27, 55, 40, 52, 53, 48, 43, 62 ],
    'ratio_necessaires': [99, 86, 57, 78, 27, 79, 87, 70, 87, 54, 93, 68, 74, 78, 82, 72, 91 ]
}

df = pd.DataFrame(data)

# Calculons la corrélation entre le temps d'exécution et chaque variable
correlations = df.corr()

# Affichons les coefficients de corrélation entre le temps d'exécution et les autres variables
print("Coefficient de corrélation entre le temps d'exécution et :")
print("- le nombre de coups total :", correlations['temps_execution']['nb_coups_total'])
print("- le nombre de coups évalués :", correlations['temps_execution']['nb_coups_evalues'])
print("- le nombre de coups nécessaires :", correlations['temps_execution']['nb_coups_necessaires'])
print("- le ratio de coups évalués :", correlations['temps_execution']['ratio_evalues'])
print("- le ratio de coups nécessaires :", correlations['temps_execution']['ratio_necessaires'])